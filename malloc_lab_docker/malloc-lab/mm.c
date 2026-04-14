/*
 * mm.c - implicit free list 기반 malloc 구현
 *
 * 이 코드는 각 블록의 header / footer에 크기와 할당 여부를 저장하고,
 * free된 블록은 앞뒤 블록과 병합(coalesce)해서 다시 사용할 수 있게 만든다.
 *
 * 핵심 동작 흐름
 * 1. mm_malloc  : 맞는 크기의 free block을 first-fit으로 찾는다.
 * 2. place      : 찾은 블록이 너무 크면 쪼개서(split) 앞부분만 할당한다.
 * 3. mm_free    : 블록을 free로 바꾸고 주변 free block과 합친다.
 * 4. extend_heap: 자리가 없으면 힙을 늘리고 새 free block을 만든다.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please  (학생에게: 다른 작업을 시작하기 전에,)
 * provide your team information in the following struct.  (아래 구조체에 팀 정보를 먼저 입력하세요)
 ********************************************************/
team_t team = {
    /* Team name */                                        /* 팀 이름 */
    "ateam",
    /* First member's full name */                         /* 첫 번째 팀원 이름 */
    "Harry Bovik",
    /* First member's email address */                     /* 첫 번째 팀원 이메일 */
    "bovik@cs.cmu.edu",
    /* Second member's full name (leave blank if none) */  /* 두 번째 팀원 이름 (없으면 빈칸) */
    "",
    /* Second member's email address (leave blank if none) */ /* 두 번째 팀원 이메일 (없으면 빈칸) */
    ""};
/*정렬 define*/
/* single word (4) or double word (8) alignment */        /* 싱글 워드(4) 또는 더블 워드(8) 정렬 */
#define ALIGNMENT 8     //  메모리 8바이트 단위로 맞춤 -> malloc은 보통 반환 주소가 정렬되어있어야함
                                            // ex) 사용자가 5바이트를 요청해도 8바이트 할당, 13바이트 요청하면 16바이트 할당

/* rounds up to the nearest multiple of ALIGNMENT */      /* size를 ALIGNMENT의 배수로 올림 */
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~0x7) // size를 8의 배수로 올림하는 매크로
                                                                                                // ex) ALIGN(1) -> 8, ALIGN(8) -> 8, ALIGN(9) -> 16, ALGIN(13) -> 16 
#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

//기본 SIZE 상수 정의
#define WSIZE 4     // header / footer 크기 -> header 크기 = 4바이트, footer 크기 = 4바이트
#define DSIZE 8     // double word 크기 8바이트 -> header + footer = 8바이트 / 최소 블록 사이즈 계산할 때 자주 쓰임
                                                                                                                            //ex) 빈 블록 하나의 최소크기 header4 + footer4 =8 
#define CHUNKSIZE (1<<12)       // 처음 늘릴 힙 크기 1 << 12 = 4096 -> 4KB / heap공간이 부족할때 -> extent_heap(CHUNKSIZE / WSIZE)
                                                        // 적당한 기본 단위로 4KB를 자주씀

#define MAX(x,y) ((x) > (y) ? (x) : (y))    // 둘 중 큰 값을 고르는 매크로 / 사용예시 : size = MAX(asize, CHUNKSIZE);
                                                                                                        // 사용자가 요청한 크기가 크면 그만큼 늘리고, 작으면 CHUNKSIZE만큼 늘림

#define PACK(size, alloc) ((size) | (alloc))        // 크기(SIZE)와 할당여부(alloc)을 한 정수에 넣는 매크로
                                                                            // ex) PACK(16, 1) -> size = 16, alloc = 1  이 두 정보를 한값으로 함침 

#define GET(p) (*(unsigned int*)(p))        // p가 가리키는 주소에서 4바이트 값을 읽어옴, 헤더나 푸터에 저장된 값을 꺼낼때 씀
                                                                          // ex) unsigned int x = GET(HDRP(bp));  -> bp 블록의 헤더 값을 읽어옴

#define PUT(p, val) (*(unsigned int*)(p) = (val))   // p가 가리키는 주소에 val값을 4바이트로 저장
                                                                                  // ex) PUT(HDRP(bp), PACK(size, 1));  -> bp 블록의 헤더에 "크기(size), 할당됨" 저장

#define GET_SIZE(p) (GET(p) & ~0x7)     // 헤더,푸터 값에서 '크기'만 꺼내는 매크로
#define GET_ALLOC(p) (GET(p) & 0x1)     // 헤더,푸터 값에서 할당 여부만 꺼내는 매크로

#define HDRP(bp) ((char *)(bp) - WSIZE)     // payload 시작 주소 bp에서 4바이트 뒤로 가면 header주소다. why? -> header는 payload 바로 앞이니까
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)    // 이 블록의 footer 주소를 구함, why? -DSIZE : payload시작에서 블록 전체 크기가면 너무 멈 
                                                                                                                // header4 + footer4 = 8byte만큼 빼주면 footer위치가 나옴,, 전체크기 - header+footer

#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))     // 현재 블록의 payload 주소 bp에서, 현재 블록 전체 크기만큼 앞으로가면 다음 블록의 payload주소다
//== #define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)))                 ex) 현재 블록 크기가 16이고, bp=1008이면,, NEXT_BLKP(bp) = 1008 +16 = 1024 ,, 즉 당므 블록의 payload 시작 주소

#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))     // 현재 블록 바로 앞에는 이전 블록의 footer가 있으니, 그 footer에서 이전 블록 크기를 읽고,
                                                                                                                                // 그 크기만큼 뒤로가면 이전 블록의 payload 주소다

/*  
 * mm_init - initialize the malloc package.               (malloc 패키지를 초기화합니다)
 */
static char *heap_listp;    // heap의 시작을 가리키는 포인터, char *인 이유는 포인터 연산을 바이트 단위로 하려고 ex) heap_listp + 4 = 4바이트 뒤의 주소

static void *coalesce(void *bp)
{
    // free한 현재 블록의 앞/뒤 블록이 할당 상태인지 먼저 확인
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {
        // case 1) 양옆이 둘 다 사용 중이면 합칠 수 없으니 그대로 반환
        return bp;
    }
    else if (prev_alloc && !next_alloc) {
        // case 2) 다음 블록이 free면 현재 블록 + 다음 블록을 하나로 합침
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    }
    else if (!prev_alloc && next_alloc) {
        // case 3) 이전 블록이 free면 이전 블록 + 현재 블록을 합침
        // 시작 주소가 이전 블록 쪽으로 바뀌므로 bp도 같이 옮겨줘야 함
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    else {
        // case 4) 앞뒤가 모두 free면 세 블록을 전부 하나로 병합
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    return bp;
}

static void *find_fit(size_t asize)
{
    // first fit
    void *bp;

    // implicit free list를 앞에서부터 끝까지 순회하면서
    // "free 상태이면서 요청 크기 이상인 첫 번째 블록"을 찾는다. -> first fit
    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)){
        if(!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp)))){
            return bp;
        }
       }
       // 끝의 epilogue(size 0)를 만날 때까지 못 찾았으면 NULL 반환
       return NULL;
       
}


static void place(void *bp, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));

    if ((csize - asize) >= (2 * DSIZE)) {
        // 남는 공간이 최소 블록 크기(16바이트) 이상이면
        // 앞부분은 할당하고, 뒷부분은 새로운 free block으로 남겨둔다.
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));

        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize - asize, 0));
        PUT(FTRP(bp), PACK(csize - asize, 0));
    }
    else {
        // 너무 애매하게 조금만 남으면 쪼개지 않고 통째로 할당
        // 작은 조각을 남기면 나중에 쓸모없는 free block이 되기 쉬움
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
}

static void *extend_heap(size_t words)
{
    char *bp;
    size_t size;

    // 힙은 보통 8바이트 정렬을 맞춰야 하므로 word 개수가 홀수면 하나 더 늘림
    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
    if((long)(bp = mem_sbrk(size)) == -1)
    {return NULL;}

    // 새로 늘어난 공간을 하나의 큰 free block으로 만들고
    // 그 뒤에는 새 epilogue header를 다시 세운다.
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));

    // 바로 앞 블록이 free였을 수도 있으니 붙여서 더 큰 free block으로 만든다.
    return coalesce(bp);
}

int mm_init(void)
{
    if((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1){     // 4*WSIZE로 힙에 16바이트 확보, mem_sbrk는 힙의 영역 확장
        return -1;}     // 실패 시 -1 리턴                                              heap_listp는 새로 확보한 16바이트의 맨 앞을 가리킴
        PUT(heap_listp, 0);     // 첫 4바이트는 alignment padding, 보통 8바이트 정렬를 맞추기 위해 둠

        PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1));    // 두번째 4바이트에 프롤로그 헤더를 씀, DSIZE가 8이면 PACK(8, 1) / 프롤로그는 실제 사용자 데이터용 블록이 아닌 경계 처리를 쉽게하려는 가짜 블록
        
        PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1));    // 세번째 4바이트에 프롤로그 푸터를 씀, 헤더와 같은 값이 들어감 / 프롤로그 블럭은 헤더4 + 푸터4 = 8byte
        
        PUT(heap_listp + (3*WSIZE), PACK(0, 1));    // 네번째 4바이트에 에필로그 헤더를 씀, PAC(0,1)은 "크기0, 할당됨" 이라는 뜻, 힙의 끝을 표시하는 표지판 역할 
                                                                                // 크기가 0인 정상 블록은 없으므로 순회할 떄 종료 조건으로 사용

        heap_listp += (2*WSIZE);    // heap_listp를 p + 8로 옮김,,프롤로그 블럭 쪽을 가리킴

        if(extend_heap(CHUNKSIZE/WSIZE) == NULL){   // 실제 사용 가능한 큰 free block을 힙 뒤에 붙임,, CHUNKSIZE(4086) / WSIZE(4) = 1024 워드만큼 확장
            return -1;}
    return 0;
}

/*
 * mm_malloc - 요청 크기에 맞는 블록을 찾아 할당합니다.
 *              없으면 힙을 확장한 뒤 할당합니다.
 */
void *mm_malloc(size_t size)
{
    size_t asize;
    size_t extendsize;
    char *bp;

    if (size == 0){
        // 0바이트 요청은 실제로 할당할 의미가 없으니 NULL 반환
        return NULL;
    }

    if (size <= DSIZE){
        // 아주 작은 요청도 header + footer + 정렬 조건 때문에
        // 최소 블록 크기인 16바이트를 차지하게 된다.
        asize = 2*DSIZE;
    }
    else
        {
        // 사용자가 요청한 payload에 header/footer 오버헤드를 더하고
        // 8바이트 배수로 올림해서 실제 블록 크기(asize)를 만든다.
        asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);
        }
        if ((bp = find_fit(asize)) != NULL){
            // 맞는 free block을 찾았으면 그 자리에 바로 배치
            place(bp, asize);
            return bp;
        }

        // 맞는 블록이 없으면 힙을 늘린다.
        // 너무 자주 늘리지 않기 위해 CHUNKSIZE와 asize 중 더 큰 쪽으로 확장
        extendsize = MAX(asize, CHUNKSIZE);
        if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
        {return NULL;}
        place(bp, asize);
        return bp; 
        }
        

/*
 * mm_free - 블록을 free 상태로 바꾸고, 인접한 free block과 병합합니다.
 */
void mm_free(void *bp)
{
    if(bp == NULL){
        // NULL free는 무시
        return;
    }

    size_t size = GET_SIZE(HDRP(bp));

    // header / footer 둘 다 "free"로 바꿔야 이 블록이 빈 공간이라고 인식된다.
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));

    // 주변에도 free block이 붙어 있으면 하나로 합쳐 단편화를 줄인다.
    coalesce(bp);
}

/*
 * mm_realloc - 새 블록을 할당하고 기존 데이터를 복사한 뒤, 이전 블록을 해제합니다.
 */
void *mm_realloc(void *ptr, size_t size)
{
    // void *oldptr = ptr;
    void *newptr; 
    void *nextbp;
    size_t asize, oldsize, nextsize,total, copySize;

    if (ptr == NULL)
    {
        return mm_malloc(size);
    }

    if (size == 0)
    {
        mm_free(ptr);
        return NULL;
    }

    if (size <= DSIZE)
    {
        asize = 2 * DSIZE;
    }
    else
    {
        asize = DSIZE * ((size + DSIZE + (DSIZE - 1)) / DSIZE);
    }
    oldsize = GET_SIZE(HDRP(ptr));

    if(oldsize >= asize)
    {
    return ptr;
    }
    nextbp = NEXT_BLKP(ptr);
    nextsize = GET_SIZE(HDRP(nextbp));

    if(nextsize == 0)
    {
        if(extend_heap((asize - oldsize) / WSIZE + 1) == NULL)
        {
            return NULL;
        }
        nextbp = NEXT_BLKP(ptr);
        nextsize = GET_SIZE(HDRP(nextbp));
    }

    if(!GET_ALLOC(HDRP(nextbp)))
    {
        total = oldsize + nextsize;

        if(total >= asize)
        {
            PUT(HDRP(ptr), PACK(total, 1));
            PUT(FTRP(ptr), PACK(total, 1));
            return ptr;
        }
    }

    newptr = mm_malloc(size);
    if(newptr == NULL)
    {
        return NULL;
    }

    copySize = oldsize - DSIZE;
    if(size < copySize)
    {
        copySize = size;
    }

    memcpy(newptr, ptr, copySize);
    mm_free(ptr);
    return newptr;
}


    // 가장 단순한 방식:
    // 1) 새 크기로 malloc
    // 2) 기존 내용 복사
    // 3) 예전 블록 free
    // newptr = mm_malloc(size);
    // if (newptr == NULL)
    //     return NULL;

    // // oldptr 바로 앞의 메타데이터 쪽에서 기존 블록 크기 정보를 읽어와
    // // 실제로 얼마만큼 복사할지 결정한다.
    // copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    // if (size < copySize)
    //     copySize = size;

    // // 새 블록이 더 작을 수도 있으므로, 둘 중 작은 크기만큼만 복사
    // memcpy(newptr, oldptr, copySize);
    // mm_free(oldptr);
    // return newptr;
