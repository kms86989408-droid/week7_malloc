/*
 * ============================================================
 *  연습문제 11 - 묵시적 가용 리스트 (Implicit Free List)
 * ============================================================
 *
 * [mm_malloc의 핵심 흐름]
 *
 *   mm_malloc(size):
 *     1. size를 8의 배수로 올림 (align)
 *     2. 힙을 처음부터 순회하며 충분히 큰 FREE 블록 탐색 (first-fit)
 *     3. 찾으면 → place() 로 블록 배치 (분할 여부 결정)
 *        못 찾으면 → extend_heap()으로 힙 확장 후 배치
 *
 * [묵시적 가용 리스트란?]
 *
 *   별도의 free list 없이, 힙의 모든 블록(할당+free)을
 *   헤더의 크기 정보만으로 순차적으로 순회합니다.
 *
 *   → Python의 리스트 전체를 for 루프로 스캔하는 것과 유사
 *   → 매우 단순하지만 O(n) 탐색 → 느림
 *
 * [first-fit 탐색]
 *
 *   bp = 첫 번째 블록
 *   while (에필로그가 아닌 동안):
 *       if (FREE이고 크기 충분):
 *           return bp   ← 발견!
 *       bp = NEXT_BLKP(bp)
 *   return NULL   ← 못 찾음
 *
 * [place: 블록 배치 및 분할]
 *
 *   찾은 free 블록 크기 = 48, 필요 크기 = 24 → 나머지 24 분할
 *
 *   분할 전: [HDR=48|FREE] [......48바이트......] [FTR=48|FREE]
 *
 *   분할 후: [HDR=24|ALLOC] [...24B...] [FTR=24|ALLOC]
 *            [HDR=24|FREE ] [...24B...] [FTR=24|FREE ]
 *
 *   단, 나머지가 너무 작으면 (< 2*DSIZE=16) 분할 안 하고 전부 사용
 *
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 매크로 정의 (10번 연습에서 완성한 것들) */
#define WSIZE     4
#define DSIZE     8
#define ALIGNMENT 8
#define MIN_BLOCK_SIZE (2 * DSIZE)   /* 최소 블록 = 헤더+푸터만 = 16바이트 */

#define PACK(size, alloc)  ((size) | (alloc))
#define GET(p)       (*(unsigned int *)(p))
#define PUT(p, val)  (*(unsigned int *)(p) = (unsigned int)(val))
#define GET_SIZE(p)  (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)
#define HDRP(bp)     ((char *)(bp) - WSIZE)
#define FTRP(bp)     ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE((char *)(bp) - DSIZE))

/* 정렬 */
#define ALIGN(size) (((size) + 7) & ~0x7)

/* -------------------------------------------------------
 * 가짜 힙 설정
 * ------------------------------------------------------- */
#define HEAP_SIZE 4096
static unsigned char fake_heap[HEAP_SIZE];
static char *heap_listp;     /* 프롤로그 블록의 bp */

/* 힙 초기화 */
void mm_init_sim(void) {
    memset(fake_heap, 0, HEAP_SIZE);
    char *p = (char *)fake_heap;

    PUT(p + 0 * WSIZE, PACK(0, 0));    /* 패딩 */
    PUT(p + 1 * WSIZE, PACK(8, 1));    /* 프롤로그 헤더 */
    PUT(p + 2 * WSIZE, PACK(8, 1));    /* 프롤로그 푸터 */
    PUT(p + 3 * WSIZE, PACK(0, 1));    /* 에필로그 헤더 */

    heap_listp = p + 2 * WSIZE;        /* 프롤로그 bp */
}

/* 힙 끝에 free 블록 추가 (extend_heap 단순화) */
char *extend_heap_sim(size_t size) {
    /* 에필로그 헤더 위치 찾기 */
    char *ep = heap_listp;
    while (GET_SIZE(HDRP(ep)) != 0) {
        ep = NEXT_BLKP(ep);
    }
    /* ep = 에필로그 bp */

    /* 에필로그 자리에 새 free 블록 설치 */
    char *new_bp = ep;
    PUT(HDRP(new_bp), PACK(size, 0));
    PUT(FTRP(new_bp), PACK(size, 0));

    /* 새 에필로그 */
    PUT(HDRP(NEXT_BLKP(new_bp)), PACK(0, 1));

    return new_bp;
}

/* -------------------------------------------------------
 * [문제 11-1] first_fit: 처음부터 순회하며 free 블록 탐색
 *
 * 조건: FREE이고, 크기가 asize 이상인 첫 번째 블록
 * 반환: 찾은 블록의 bp, 없으면 NULL
 * ------------------------------------------------------- */
char *first_fit(size_t asize) {
    char *bp = heap_listp;

    /*
     * 에필로그(크기==0)를 만날 때까지 순회
     *
     * 탈출 조건: GET_SIZE(HDRP(bp)) == 0  → 에필로그
     *
     * 조건 확인:
     *   1. 할당 안 된 블록인가? → GET_ALLOC(HDRP(bp)) == 0
     *   2. 크기가 충분한가?    → GET_SIZE(HDRP(bp)) >= asize
     *   두 조건 모두 만족하면 bp 반환
     */
    while (/* TODO: 에필로그가 아닌 동안 (크기 != 0) */) {

        /* TODO: FREE이고 크기가 충분하면 bp를 반환하세요 */
        if (/* ??? */ && /* ??? */) {
            return bp;
        }

        /* TODO: 다음 블록으로 이동 */
        bp = /* ??? */;
    }

    return NULL;  /* 적합한 블록 없음 */
}

/* -------------------------------------------------------
 * [문제 11-2] place: 블록 배치 (분할 포함)
 *
 * bp: 배치할 free 블록의 bp
 * asize: 필요한 크기 (이미 정렬된 값)
 *
 * 로직:
 *   남은 크기 = 현재 블록 크기 - asize
 *   if (남은 크기 >= MIN_BLOCK_SIZE):  ← 분할 가능
 *       앞부분 asize를 할당 블록으로 표시
 *       뒷부분 (남은 크기)을 free 블록으로 표시
 *   else:  ← 분할 불가 → 블록 전체 사용
 *       현재 블록 전체를 할당 블록으로 표시
 * ------------------------------------------------------- */
void place(char *bp, size_t asize) {
    size_t csize = GET_SIZE(HDRP(bp));   /* 현재 free 블록의 크기 */
    size_t remain = csize - asize;        /* 분할 후 남는 크기 */

    if (remain >= MIN_BLOCK_SIZE) {
        /* 분할: 앞부분 asize 할당 */
        /* TODO: bp의 헤더와 푸터에 PACK(asize, 1)을 PUT하세요 */
        PUT(HDRP(bp), /* ??? */);
        PUT(FTRP(bp), /* ??? */);

        /* 분할: 뒷부분 remain free */
        char *next = NEXT_BLKP(bp);
        /* TODO: 뒷부분의 헤더와 푸터에 PACK(remain, 0)을 PUT하세요 */
        PUT(HDRP(next), /* ??? */);
        PUT(FTRP(next), /* ??? */);

    } else {
        /* 분할 안 함: 전체 블록 할당 */
        /* TODO: bp의 헤더와 푸터에 전체 크기(csize)로 할당 표시 */
        PUT(HDRP(bp), /* ??? */);
        PUT(FTRP(bp), /* ??? */);
    }
}

/* -------------------------------------------------------
 * [문제 11-3] mm_malloc 완성
 *
 * 실제 mm_malloc의 핵심 로직:
 *   1. 요청 크기를 정렬 + 헤더/푸터 포함 크기로 조정
 *   2. first_fit으로 free 블록 탐색
 *   3. 찾으면 place, 못 찾으면 extend 후 place
 * ------------------------------------------------------- */
void *mm_malloc_sim(size_t size) {
    if (size == 0) return NULL;

    /* 크기 조정: 헤더+푸터(8B) + 정렬 */
    size_t asize;
    if (size <= DSIZE) {
        asize = MIN_BLOCK_SIZE;   /* 최소 16바이트 */
    } else {
        /* (size + DSIZE + DSIZE - 1) & ~0x7 */
        asize = ALIGN(size + DSIZE);
    }

    /* TODO: first_fit으로 적합한 블록 탐색 */
    char *bp = /* ??? */;

    if (bp != NULL) {
        /* TODO: 찾은 블록에 place */
        /* ??? */;
        return (void *)bp;
    }

    /* free 블록 없음 → 힙 확장 */
    bp = extend_heap_sim(asize < 4096 ? 4096 : asize);
    if (bp == NULL) return NULL;

    place(bp, asize);
    return (void *)bp;
}

/* 블록 free (단순 버전, coalesce 없음) */
void mm_free_sim(void *ptr) {
    if (ptr == NULL) return;
    size_t size = GET_SIZE(HDRP(ptr));
    /* TODO: 헤더와 푸터를 PACK(size, 0)으로 업데이트하세요 */
    PUT(HDRP(ptr), /* ??? */);
    PUT(FTRP(ptr), /* ??? */);
}

/* 힙 상태 출력 */
void print_heap(void) {
    printf("\n--- 힙 상태 ---\n");
    char *bp = heap_listp;
    int  blk = 0;
    while (GET_SIZE(HDRP(bp)) != 0) {
        printf("  블록[%d] 크기=%-4zu %s\n",
               blk++,
               GET_SIZE(HDRP(bp)),
               GET_ALLOC(HDRP(bp)) ? "[할당]" : "[FREE]");
        bp = NEXT_BLKP(bp);
    }
    printf("  [에필로그]\n");
}

int main(void) {

    mm_init_sim();
    extend_heap_sim(4096);   /* 4KB free 블록 추가 */

    printf("=== [문제 11-1] first_fit 테스트 ===\n");

    void *p1 = mm_malloc_sim(24);   /* 24+8=32바이트 블록 */
    printf("malloc(24) → bp=%p\n", p1);

    void *p2 = mm_malloc_sim(48);
    printf("malloc(48) → bp=%p\n", p2);

    void *p3 = mm_malloc_sim(16);
    printf("malloc(16) → bp=%p\n", p3);

    print_heap();

    printf("\n=== [문제 11-2] free 후 재할당 ===\n");
    mm_free_sim(p2);
    printf("free(p2=%p) 완료\n", p2);
    print_heap();

    void *p4 = mm_malloc_sim(32);   /* p2 자리(56B)에 들어갈 수 있음 */
    printf("malloc(32) → bp=%p (p2=%p와 같으면 재사용 성공)\n", p4, p2);
    print_heap();

    return 0;
}

/*
 * [컴파일 & 실행 방법]
 *   gcc -o 11_implicit 11_implicit_list.c
 *   ./11_implicit
 *
 * [mm-lab 연결고리]
 *   mm.c에서 구현할 핵심 함수:
 *   - mm_init: 힙 초기화 (프롤로그, 에필로그 설치)
 *   - find_fit(first_fit): 가용 블록 탐색
 *   - place: 블록 배치 + 분할
 *   - mm_malloc: 위를 조합
 *   - mm_free: 헤더/푸터 갱신 후 coalesce 호출
 *
 * [생각해보기]
 *   - first_fit 대신 best_fit을 쓰면 어떻게 달라질까?
 *   - place에서 분할 기준을 MIN_BLOCK_SIZE보다 크게 잡으면?
 *   - free 후 coalesce를 안 하면 어떤 문제가 생길까?
 */
