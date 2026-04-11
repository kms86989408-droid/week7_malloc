/*
 * ============================================================
 *  연습문제 12 - Coalescing (인접 블록 병합)
 * ============================================================
 *
 * [왜 필요한가?]
 *
 * free를 여러 번 하면 힙에 작은 free 블록들이 흩어집니다.
 *
 *   [ALLOC 32] [FREE 16] [FREE 16] [ALLOC 24]
 *
 * 32바이트를 malloc하려고 하면?
 *   → first_fit이 free(16)을 찾지만 너무 작음 → 실패!
 *   → 힙을 확장해서 새로 만듦 → 메모리 낭비!
 *
 * coalesce(병합)를 하면:
 *   [ALLOC 32] [FREE  32 ] [ALLOC 24]
 *   → 32바이트 가용 블록 생성 → 재사용 가능!
 *
 * [4가지 경우]
 *
 * 현재 블록을 free할 때, 이웃 블록의 상태에 따라:
 *
 * Case 1: 이전=ALLOC, 다음=ALLOC
 *   → 병합 없음. 현재 블록만 free 표시
 *
 * Case 2: 이전=ALLOC, 다음=FREE
 *   → 현재 + 다음 병합
 *   → 새 크기 = 현재 크기 + 다음 크기
 *   → 현재 헤더 + 다음 푸터에 새 크기 기록
 *
 * Case 3: 이전=FREE, 다음=ALLOC
 *   → 이전 + 현재 병합
 *   → 새 크기 = 이전 크기 + 현재 크기
 *   → 이전 헤더 + 현재 푸터에 새 크기 기록
 *
 * Case 4: 이전=FREE, 다음=FREE
 *   → 이전 + 현재 + 다음 3개 병합
 *   → 새 크기 = 이전 + 현재 + 다음
 *   → 이전 헤더 + 다음 푸터에 새 크기 기록
 *
 * [시각화]
 *
 * Case 2:
 *   [ALLOC] [curr-FREE] [next-FREE]
 *            ↓
 *   [ALLOC] [    curr+next FREE   ]
 *
 * Case 3:
 *   [prev-FREE] [curr-FREE] [ALLOC]
 *        ↓
 *   [  prev+curr FREE  ] [ALLOC]
 *
 * Case 4:
 *   [prev-FREE] [curr-FREE] [next-FREE]
 *        ↓
 *   [      prev+curr+next FREE       ]
 *
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 매크로 (앞 연습과 동일) */
#define WSIZE     4
#define DSIZE     8
#define MIN_BLOCK_SIZE (2 * DSIZE)

#define PACK(size, alloc)  ((size) | (alloc))
#define GET(p)       (*(unsigned int *)(p))
#define PUT(p, val)  (*(unsigned int *)(p) = (unsigned int)(val))
#define GET_SIZE(p)  (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)
#define HDRP(bp)     ((char *)(bp) - WSIZE)
#define FTRP(bp)     ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE((char *)(bp) - DSIZE))

/* -------------------------------------------------------
 * 가짜 힙 설정
 * ------------------------------------------------------- */
#define HEAP_SIZE 4096
static unsigned char fake_heap[HEAP_SIZE];
static char *heap_listp;

void mm_init_sim(void) {
    memset(fake_heap, 0, HEAP_SIZE);
    char *p = (char *)fake_heap;
    PUT(p + 0 * WSIZE, PACK(0, 0));
    PUT(p + 1 * WSIZE, PACK(8, 1));
    PUT(p + 2 * WSIZE, PACK(8, 1));
    PUT(p + 3 * WSIZE, PACK(0, 1));
    heap_listp = p + 2 * WSIZE;
}

/* 힙에 블록 순차적으로 추가 (테스트용) */
static char *heap_end = NULL;

void setup_block(size_t size, int alloc) {
    if (heap_end == NULL) {
        /* 에필로그 위치 찾기 */
        heap_end = heap_listp;
        while (GET_SIZE(HDRP(heap_end)) != 0)
            heap_end = NEXT_BLKP(heap_end);
        /* heap_end는 에필로그 bp */
    }
    char *bp = heap_end;
    PUT(HDRP(bp), PACK(size, alloc));
    PUT(FTRP(bp), PACK(size, alloc));
    heap_end = NEXT_BLKP(bp);
    PUT(HDRP(heap_end), PACK(0, 1));   /* 에필로그 갱신 */
}

/* 힙 상태 출력 */
void print_heap(const char *label) {
    printf("\n[%s]\n", label);
    char *bp = heap_listp;
    while (GET_SIZE(HDRP(bp)) != 0) {
        printf("  [크기=%-4zu %s]\n",
               GET_SIZE(HDRP(bp)),
               GET_ALLOC(HDRP(bp)) ? "ALLOC" : "FREE ");
        bp = NEXT_BLKP(bp);
    }
    printf("  [에필로그]\n");
}

/* -------------------------------------------------------
 * [문제 12-1] coalesce 함수 완성
 *
 * mm_free에서 free 표시 후 이 함수를 호출합니다.
 * bp: 방금 free된 블록의 bp
 * 반환: 병합 후 최종 블록의 bp
 * ------------------------------------------------------- */
void *coalesce(void *bp) {

    /* 이전/다음 블록의 할당 상태 확인 */
    /* TODO: 이전 블록이 할당됐는지 확인 */
    int prev_alloc = GET_ALLOC(/* ??? */);   /* 이전 블록의 푸터 */

    /* TODO: 다음 블록이 할당됐는지 확인 */
    int next_alloc = GET_ALLOC(/* ??? */);   /* 다음 블록의 헤더 */

    /* 현재 블록 크기 */
    size_t size = GET_SIZE(HDRP(bp));

    /* -------------------------------------------------------
     * Case 1: 이전=ALLOC, 다음=ALLOC
     * 병합 없음. 그냥 반환.
     * ------------------------------------------------------- */
    if (prev_alloc && next_alloc) {
        return bp;   /* TODO: 그대로 반환 */
    }

    /* -------------------------------------------------------
     * Case 2: 이전=ALLOC, 다음=FREE
     * 현재 + 다음 블록 병합
     * ------------------------------------------------------- */
    else if (prev_alloc && !next_alloc) {
        /* 새 크기 = 현재 + 다음 */
        /* TODO: size에 다음 블록의 크기를 더하세요 */
        size += /* ??? */;

        /* TODO: 현재 블록 헤더를 새 크기(free)로 업데이트 */
        PUT(HDRP(bp), /* ??? */);

        /* TODO: 현재 블록의 새 푸터(병합 후 끝)를 업데이트 */
        /* 힌트: FTRP(bp)는 헤더 갱신 후 자동으로 새 끝을 가리킴 */
        PUT(FTRP(bp), /* ??? */);
    }

    /* -------------------------------------------------------
     * Case 3: 이전=FREE, 다음=ALLOC
     * 이전 + 현재 블록 병합
     * ------------------------------------------------------- */
    else if (!prev_alloc && next_alloc) {
        /* 새 크기 = 이전 + 현재 */
        /* TODO: size에 이전 블록의 크기를 더하세요 */
        size += /* ??? */;

        /* TODO: 현재 블록의 푸터를 새 크기(free)로 업데이트 */
        PUT(FTRP(bp), /* ??? */);

        /* bp를 이전 블록으로 이동 */
        /* TODO: bp를 이전 블록의 bp로 이동하세요 */
        bp = /* ??? */;

        /* TODO: 이전 블록의 헤더를 새 크기(free)로 업데이트 */
        PUT(HDRP(bp), /* ??? */);
    }

    /* -------------------------------------------------------
     * Case 4: 이전=FREE, 다음=FREE
     * 이전 + 현재 + 다음 병합
     * ------------------------------------------------------- */
    else {
        /* 새 크기 = 이전 + 현재 + 다음 */
        /* TODO: size에 이전 블록 크기와 다음 블록 크기 모두 더하세요 */
        size += /* ??? */ + /* ??? */;

        /* bp를 이전 블록으로 이동 */
        bp = /* ??? */;

        /* TODO: 이전 블록의 헤더를 새 크기(free)로 업데이트 */
        PUT(HDRP(bp), /* ??? */);

        /* TODO: 다음 블록의 푸터(= 병합된 블록의 끝)를 업데이트 */
        /* 힌트: FTRP(bp)가 이제 세 블록을 합친 끝을 가리킴 */
        PUT(FTRP(bp), /* ??? */);
    }

    return bp;   /* 병합된 블록의 bp 반환 */
}

/* mm_free: free 표시 후 coalesce 호출 */
void mm_free_sim(void *ptr) {
    if (ptr == NULL) return;
    size_t size = GET_SIZE(HDRP(ptr));
    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));
    coalesce(ptr);   /* ← 병합! */
}

int main(void) {

    /* -------------------------------------------------------
     * [테스트 1] Case 2: 현재 + 다음 병합
     *
     * 초기 힙:
     *   [ALLOC 32] [ALLOC 24] [ALLOC 32]
     *
     * 가운데를 free:
     *   [ALLOC 32] [FREE  24] [ALLOC 32]
     *   → Case 1, 병합 없음
     *
     * 마지막을 free:
     *   [ALLOC 32] [FREE  24] [FREE  32]
     *   → 가운데를 다시 free하면 Case 2: 24+32=56으로 병합
     * ------------------------------------------------------- */
    printf("=== [테스트 1] Case 2: 현재+다음 병합 ===\n");
    mm_init_sim();
    heap_end = NULL;

    setup_block(32, 1);
    setup_block(24, 1);
    setup_block(32, 1);
    print_heap("초기 상태");

    /* 힙에서 두 번째 블록 bp 찾기 */
    char *blk1 = heap_listp;         /* 프롤로그 */
    char *blk2 = NEXT_BLKP(blk1);    /* 32 ALLOC */
    char *blk3 = NEXT_BLKP(blk2);    /* 24 ALLOC */
    char *blk4 = NEXT_BLKP(blk3);    /* 32 ALLOC */

    mm_free_sim(blk4);   /* 마지막 free */
    print_heap("blk4(32) free 후");

    mm_free_sim(blk3);   /* 가운데 free → Case 2: 24+32=56 병합 */
    print_heap("blk3(24) free 후 (Case 2: 24+32=56 예상)");

    /* -------------------------------------------------------
     * [테스트 2] Case 3: 이전 + 현재 병합
     * ------------------------------------------------------- */
    printf("\n=== [테스트 2] Case 3: 이전+현재 병합 ===\n");
    mm_init_sim();
    heap_end = NULL;

    setup_block(32, 1);
    setup_block(24, 1);
    setup_block(32, 1);
    print_heap("초기 상태");

    blk2 = NEXT_BLKP(heap_listp);
    blk3 = NEXT_BLKP(blk2);
    blk4 = NEXT_BLKP(blk3);

    mm_free_sim(blk2);   /* 첫 번째 free */
    print_heap("blk2(32) free 후");

    mm_free_sim(blk3);   /* 두 번째 free → Case 3: 32+24=56 병합 */
    print_heap("blk3(24) free 후 (Case 3: 32+24=56 예상)");

    /* -------------------------------------------------------
     * [테스트 3] Case 4: 이전 + 현재 + 다음 병합
     * ------------------------------------------------------- */
    printf("\n=== [테스트 3] Case 4: 3개 병합 ===\n");
    mm_init_sim();
    heap_end = NULL;

    setup_block(32, 1);
    setup_block(24, 1);
    setup_block(32, 1);
    print_heap("초기 상태");

    blk2 = NEXT_BLKP(heap_listp);
    blk3 = NEXT_BLKP(blk2);
    blk4 = NEXT_BLKP(blk3);

    mm_free_sim(blk2);   /* 32 free */
    mm_free_sim(blk4);   /* 32 free */
    print_heap("blk2, blk4 free 후");

    mm_free_sim(blk3);   /* 가운데 free → Case 4: 32+24+32=88 병합 */
    print_heap("blk3 free 후 (Case 4: 32+24+32=88 예상)");

    return 0;
}

/*
 * [컴파일 & 실행 방법]
 *   gcc -o 12_coalesce 12_coalesce.c
 *   ./12_coalesce
 *
 * [mm-lab 연결고리]
 *   mm_free 구현:
 *     void mm_free(void *ptr) {
 *         size_t size = GET_SIZE(HDRP(ptr));
 *         PUT(HDRP(ptr), PACK(size, 0));
 *         PUT(FTRP(ptr), PACK(size, 0));
 *         coalesce(ptr);           ← 이 함수!
 *     }
 *
 *   coalesce에서 4가지 케이스를 if-else로 처리합니다.
 *   Case 판단: prev_alloc, next_alloc 두 변수로 결정.
 *
 * [생각해보기]
 *   - Case 2에서 FTRP(bp)를 헤더 갱신 전에 계산하면 어떻게 되나?
 *   - 프롤로그/에필로그가 없으면 경계 체크를 어떻게 해야 할까?
 *   - coalesce를 안 하면 점수가 몇 점이나 떨어질까? (mm-lab 채점 기준)
 */
