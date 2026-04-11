/*
 * ============================================================
 *  연습문제 10 - 힙 블록 구조 & 매크로 완성
 * ============================================================
 *
 * [malloc-lab의 블록 레이아웃]
 *
 * 각 블록의 구조 (Implicit Free List 방식):
 *
 *  ┌─────────┬──────────────────────────────┬─────────┐
 *  │ HEADER  │        PAYLOAD               │ FOOTER  │
 *  │  4 byte │     (요청한 크기 + 패딩)      │  4 byte │
 *  └─────────┴──────────────────────────────┴─────────┘
 *  ↑         ↑                              ↑
 *  bp-4      bp (블록 포인터, 사용자에게 반환) bp+size-8
 *
 * 헤더 = 푸터 = PACK(size, alloc)
 *
 * [힙 전체 레이아웃]
 *
 *  ┌────────┬────────────┬─────────────┬──── ... ────┬───────────┐
 *  │ 패딩   │ 프롤로그HDR │ 프롤로그FTR │   블록들    │ 에필로그  │
 *  │ (4B)  │ PACK(8,1)  │ PACK(8,1)  │             │ PACK(0,1) │
 *  └────────┴────────────┴─────────────┴──── ... ────┴───────────┘
 *  ↑mem_lo                                            ↑mem_hi
 *
 *  프롤로그: 항상 할당된 8바이트 블록 (헤더+푸터만, 페이로드 없음)
 *  에필로그: 크기=0인 할당 블록 (헤더만) → 순회 종료 신호
 *
 * [mm_init에서 하는 일]
 *   1. mem_sbrk(4 * WSIZE)로 16바이트 확보
 *   2. 패딩(4B) + 프롤로그 헤더(4B) + 프롤로그 푸터(4B) + 에필로그(4B) 설치
 *   3. heap_listp = 프롤로그 푸터 주소 (bp 역할)
 *
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* -------------------------------------------------------
 * 매크로 정의 (TODO 부분을 채워서 완성하세요)
 * ------------------------------------------------------- */

#define WSIZE     4    /* 워드 크기 (bytes) */
#define DSIZE     8    /* 더블 워드 크기 */
#define CHUNKSIZE (1 << 12)  /* 힙 확장 단위 (4096 bytes) */

/* 크기와 할당비트를 합치기 */
/* TODO: PACK 매크로를 완성하세요 (08번에서 이미 해봤음!) */
#define PACK(size, alloc)  /* ??? */

/* 포인터 p 위치에 4바이트 읽기/쓰기 */
#define GET(p)       (*(unsigned int *)(p))
#define PUT(p, val)  (*(unsigned int *)(p) = (unsigned int)(val))

/* 헤더/푸터에서 크기와 alloc비트 추출 */
/* TODO: GET_SIZE와 GET_ALLOC을 완성하세요 (08번에서 이미 해봤음!) */
#define GET_SIZE(p)   (GET(p) & /* ??? */)
#define GET_ALLOC(p)  (GET(p) & /* ??? */)

/* 블록 포인터 bp로부터 헤더/푸터 주소 계산 */
/* TODO: HDRP를 완성하세요 */
/* 힌트: bp에서 WSIZE(4바이트) 앞으로 이동 */
#define HDRP(bp)  ((char *)(bp) - /* ??? */)

/* TODO: FTRP를 완성하세요 */
/* 힌트: bp에서 (블록크기 - DSIZE)만큼 뒤로 이동 */
/* FTRP(bp) = (char*)(bp) + GET_SIZE(HDRP(bp)) - DSIZE */
#define FTRP(bp)  ((char *)(bp) + GET_SIZE(HDRP(bp)) - /* ??? */)

/* 다음 블록의 bp 계산 */
/* TODO: NEXT_BLKP를 완성하세요 */
/* 힌트: 현재 bp에서 현재 블록 크기만큼 앞으로 이동 */
#define NEXT_BLKP(bp)  ((char *)(bp) + /* ??? */)

/* 이전 블록의 bp 계산 */
/* TODO: PREV_BLKP를 완성하세요 */
/* 힌트: 현재 bp에서 이전 블록의 크기만큼 뒤로 이동 */
/* 이전 블록의 크기는 현재 bp - DSIZE 위치(이전 블록의 푸터)에 있음 */
#define PREV_BLKP(bp)  ((char *)(bp) - GET_SIZE(/* ??? */))

/* -------------------------------------------------------
 * 가짜 힙 시뮬레이터
 * 실제 mem_sbrk 대신 배열로 힙을 흉내냅니다.
 * ------------------------------------------------------- */

#define HEAP_SIZE 256
static unsigned char fake_heap[HEAP_SIZE];  /* 힙 바이트 배열 */

/* 힙 초기화: mm_init과 동일한 구조로 설치 */
static char *heap_listp;   /* 프롤로그 블록의 bp */

void sim_init(void) {
    memset(fake_heap, 0, HEAP_SIZE);
    char *p = (char *)fake_heap;

    /*
     * 힙 초기 구조 설치:
     * [0~3]   = 패딩 (정렬용, 0으로)
     * [4~7]   = 프롤로그 헤더  PACK(8, 1)
     * [8~11]  = 프롤로그 푸터  PACK(8, 1)
     * [12~15] = 에필로그 헤더  PACK(0, 1)
     */

    /* TODO: 패딩 영역(p+0)에 0을 PUT하세요 */
    PUT(p + 0 * WSIZE, /* ??? */);

    /* TODO: 프롤로그 헤더(p+4)에 PACK(8, 1)을 PUT하세요 */
    PUT(p + 1 * WSIZE, /* ??? */);

    /* TODO: 프롤로그 푸터(p+8)에 PACK(8, 1)을 PUT하세요 */
    PUT(p + 2 * WSIZE, /* ??? */);

    /* TODO: 에필로그 헤더(p+12)에 PACK(0, 1)을 PUT하세요 */
    PUT(p + 3 * WSIZE, /* ??? */);

    /* heap_listp = 프롤로그 푸터의 다음 = p+8+4 = p+12? NO!
     * heap_listp는 프롤로그 블록의 bp = 프롤로그 헤더 다음
     * p+4(HDR) 다음 = p+8 = 프롤로그의 페이로드 시작 (= 푸터)
     *
     * 즉 heap_listp = p + 2*WSIZE = p+8
     */
    heap_listp = p + 2 * WSIZE;
}

/* 힙에 새 블록을 추가하는 함수 (extend_heap 단순화 버전) */
char *sim_alloc_block(size_t size, int alloc) {
    /*
     * 현재 에필로그 헤더 위치를 찾아서 거기서부터 블록을 씀
     * 실제 mm_malloc에선 extend_heap + mem_sbrk를 씀
     */
    static char *next_free = NULL;
    if (next_free == NULL) {
        next_free = (char *)fake_heap + 4 * WSIZE;  /* 에필로그 위치 */
    }

    char *bp = next_free + WSIZE;   /* 헤더 다음 = bp */

    /* 헤더와 푸터 설치 */
    PUT(HDRP(bp), PACK(size, alloc));
    PUT(FTRP(bp), PACK(size, alloc));

    /* 에필로그를 블록 뒤로 이동 */
    next_free = (char *)NEXT_BLKP(bp) - WSIZE;
    PUT(next_free, PACK(0, 1));

    return bp;
}

/* 힙의 블록들을 순서대로 출력 */
void print_heap(void) {
    printf("\n=== 힙 블록 순회 ===\n");
    char *bp = heap_listp;   /* 프롤로그 bp에서 시작 */

    while (1) {
        size_t size  = GET_SIZE(HDRP(bp));
        int    alloc = GET_ALLOC(HDRP(bp));

        if (size == 0) {  /* 에필로그 */
            printf("[에필로그] 순회 종료\n");
            break;
        }

        printf("[블록] bp=%p, 크기=%zu, %s\n",
               (void*)bp, size, alloc ? "할당됨" : "FREE");

        /* TODO: 다음 블록으로 이동하세요 */
        bp = /* ??? */;
    }
}

int main(void) {

    /* 힙 초기화 */
    sim_init();

    printf("=== [문제 10-1] 힙 초기 상태 확인 ===\n");
    printf("heap_listp(프롤로그 bp): %p\n", (void*)heap_listp);
    printf("프롤로그 헤더 값: 0x%X (예상: 0x9 = PACK(8,1))\n",
           GET(HDRP(heap_listp)));
    printf("프롤로그 푸터 값: 0x%X (예상: 0x9)\n",
           GET(FTRP(heap_listp)));

    /* -------------------------------------------------------
     * [문제 10-2] 블록 할당하고 매크로 검증
     * ------------------------------------------------------- */
    printf("\n=== [문제 10-2] 블록 배치 및 매크로 검증 ===\n");

    char *blk1 = sim_alloc_block(24, 1);  /* 24바이트 할당 블록 */
    char *blk2 = sim_alloc_block(32, 0);  /* 32바이트 free 블록 */
    char *blk3 = sim_alloc_block(16, 1);  /* 16바이트 할당 블록 */

    /* blk1 검증 */
    printf("\nblk1:\n");
    printf("  bp주소:         %p\n", (void*)blk1);

    /* TODO: HDRP(blk1) 주소 출력 */
    printf("  헤더 주소:      %p\n", (void*)(/* ??? */));

    /* TODO: FTRP(blk1) 주소 출력 */
    printf("  푸터 주소:      %p\n", (void*)(/* ??? */));

    /* TODO: 헤더에서 크기 추출 */
    printf("  블록 크기:      %u (예상: 24)\n",
           GET_SIZE(/* ??? */));

    /* TODO: 헤더에서 alloc 추출 */
    printf("  할당 여부:      %u (예상: 1)\n",
           GET_ALLOC(/* ??? */));

    /* blk2 검증 */
    printf("\nblk2:\n");
    /* TODO: blk2의 크기와 alloc을 출력하세요 */
    printf("  블록 크기:      %u (예상: 32)\n", GET_SIZE(/* ??? */));
    printf("  할당 여부:      %u (예상: 0)\n",  GET_ALLOC(/* ??? */));

    /* -------------------------------------------------------
     * [문제 10-3] NEXT_BLKP / PREV_BLKP 검증
     * ------------------------------------------------------- */
    printf("\n=== [문제 10-3] NEXT/PREV 포인터 검증 ===\n");

    /* TODO: blk1의 다음 블록이 blk2인지 확인 */
    char *next_of_blk1 = /* ??? */;
    printf("blk1의 다음 블록: %p\n", (void*)next_of_blk1);
    printf("blk2:             %p\n", (void*)blk2);
    printf("일치: %s\n", (next_of_blk1 == blk2) ? "YES ✓" : "NO ✗");

    /* TODO: blk3의 이전 블록이 blk2인지 확인 */
    char *prev_of_blk3 = /* ??? */;
    printf("\nblk3의 이전 블록: %p\n", (void*)prev_of_blk3);
    printf("blk2:              %p\n", (void*)blk2);
    printf("일치: %s\n", (prev_of_blk3 == blk2) ? "YES ✓" : "NO ✗");

    /* 전체 힙 순회 */
    print_heap();

    return 0;
}

/*
 * [컴파일 & 실행 방법]
 *   gcc -o 10_heap 10_heap_block.c
 *   ./10_heap
 *
 * [mm-lab 연결고리]
 *   이 파일의 매크로들이 mm.c 에 그대로 들어갑니다.
 *   PACK, GET, PUT, GET_SIZE, GET_ALLOC, HDRP, FTRP,
 *   NEXT_BLKP, PREV_BLKP → 이 8개가 핵심 매크로입니다.
 *
 * [생각해보기]
 *   - 에필로그가 PACK(0, 1)인 이유는? (크기가 왜 0이지?)
 *   - 프롤로그가 필요한 이유는? (없으면 PREV_BLKP 계산이 어떻게 됨?)
 *   - 푸터가 없으면 PREV_BLKP를 어떻게 계산할 수 있을까? (심화)
 */
