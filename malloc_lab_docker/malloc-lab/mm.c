/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.      (가장 빠르지만 메모리 효율이 가장 낮은 malloc 패키지)
 *
 * In this naive approach, a block is allocated by simply incrementing   (이 단순한 방식에서는 brk 포인터를 증가시키는 것만으로 블록을 할당합니다)
 * the brk pointer.  A block is pure payload. There are no headers or    (블록은 순수한 페이로드만으로 구성됩니다. 헤더나 푸터가 없습니다)
 * footers.  Blocks are never coalesced or reused. Realloc is            (블록은 절대 병합(coalesce)되거나 재사용되지 않습니다. realloc은)
 * implemented directly using mm_malloc and mm_free.                     (mm_malloc과 mm_free를 이용해 직접 구현됩니다)
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header    (학생에게: 이 헤더 주석을 여러분의 솔루션에 대한)
 * comment that gives a high level description of your solution.         (고수준 설명으로 교체하세요)
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
            return 0;}
    return 0;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.        (brk 포인터를 증가시켜 블록을 할당합니다)
 *     Always allocate a block whose size is a multiple of the alignment.(항상 크기가 정렬의 배수인 블록을 할당합니다)
 */
void *mm_malloc(size_t size)
{
    size_t asize;
    size_t extendsize;
    char *ptr;

    if (size == 0){
        return NULL;
    }

    if (size <= DSIZE){
        asize = 2*DSIZE;
    }
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);

        if ((ptr = find_fit(asize)) != NULL){
            place(ptr, asize);
            return ptr;
        }

        if ((ptr = extend_heap(extendsize/WSIZE)) == NULL)
        return NULL;
        place(ptr, asize);
        return ptr; 
}

/*
 * mm_free - Freeing a block does nothing.                (블록을 해제하지만 아무것도 하지 않습니다)
 */
void mm_free(void *ptr)
{
    if(ptr == NULL){
        return;
    }

    size_t size = GET_SIZE(HDRP(ptr));

    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));
    coalesce(ptr);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free     (mm_malloc과 mm_free를 이용해 단순하게 구현됩니다)
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;

    newptr = mm_malloc(size);
    if (newptr == NULL)
        return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
        copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}
