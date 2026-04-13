/*
 * ============================================================
 *  연습문제 09 - void* 포인터 연산 (malloc-lab 핵심!)
 * ============================================================
 *
 * [왜 필요한가?]
 *
 * mm.c에서 힙을 바이트 단위로 이동할 때 이런 코드가 등장합니다:
 *
 *   char *bp;            // 블록 포인터
 *   bp = (char *)p + 4; // 4바이트 앞으로 이동
 *
 * 또는 매크로로:
 *   #define HDRP(bp) ((char *)(bp) - WSIZE)
 *   → bp에서 4바이트 뒤(헤더 위치)로 이동
 *
 * [핵심 규칙]
 *
 *   void *p = ...;
 *   p + 1    → 컴파일 오류! void*는 크기 모름 → 연산 불가
 *   (char*)p + 1  → OK! char는 1바이트 → 1바이트 이동
 *   (int*)p  + 1  → OK! int는  4바이트 → 4바이트 이동
 *
 * [포인터 + N 의 의미]
 *   ptr + N  =  ptr의 주소 + (N × sizeof(*ptr))
 *
 *   char *p = 0x100;
 *   p + 1  → 0x101  (1 × 1 = 1바이트 이동)
 *   p + 4  → 0x104  (4 × 1 = 4바이트 이동)
 *
 *   int *p = 0x100;
 *   p + 1  → 0x104  (1 × 4 = 4바이트 이동)
 *   p + 4  → 0x110  (4 × 4 = 16바이트 이동)
 *
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>

/* malloc-lab에서 사용하는 워드 크기 */
#define WSIZE  4   /* 4바이트 = 헤더/푸터 크기 */
#define DSIZE  8   /* 8바이트 = 더블워드 */

int main(void) {

    /* -------------------------------------------------------
     * [문제 9-1] char* vs int* 포인터 연산 차이 이해
     *
     * 같은 숫자를 더해도 이동하는 바이트 수가 다릅니다!
     * ------------------------------------------------------- */
    printf("=== [문제 9-1] 포인터 타입별 이동 크기 ===\n");

    char  *cptr = (char  *)0x1000;
    int   *iptr = (int   *)0x1000;
    long  *lptr = (long  *)0x1000;

    /* TODO: 각각 +1 했을 때 주소가 얼마인지 출력하세요 */
    /* 힌트: (long)(cptr + 1) 처럼 캐스팅해서 숫자로 출력 */
    printf("char*  0x1000 + 1 = 0x%lX (예상: 0x1001, 1바이트 이동)\n",
           (long)(/* ??? */));
    printf("int*   0x1000 + 1 = 0x%lX (예상: 0x1004, 4바이트 이동)\n",
           (long)(/* ??? */));
    printf("long*  0x1000 + 1 = 0x%lX (예상: 0x1008, 8바이트 이동)\n",
           (long)(/* ??? */));

    /* -------------------------------------------------------
     * [문제 9-2] malloc-lab 매크로 시뮬레이션
     *
     * 실제 mm.c에서 쓰이는 블록 포인터 매크로:
     *
     * bp = 블록 포인터 (페이로드 시작 주소)
     *
     * [블록 레이아웃]
     * 주소:  [bp-4] [bp] ................. [bp+size-8] [bp+size-4]
     * 내용:  [HDR ] [payload............] [    footer            ]
     *
     * HDRP(bp) = (char*)(bp) - WSIZE      ← 헤더 주소
     * FTRP(bp) = (char*)(bp) + 크기 - DSIZE ← 푸터 주소
     *
     * 다음 블록의 bp:
     * NEXT_BLKP(bp) = (char*)(bp) + GET_SIZE(HDRP(bp))
     *
     * 이전 블록의 bp:
     * PREV_BLKP(bp) = (char*)(bp) - GET_SIZE((char*)(bp) - DSIZE)
     * ------------------------------------------------------- */
    printf("\n=== [문제 9-2] 블록 포인터 매크로 계산 ===\n");

    /*
     * 힙을 배열로 시뮬레이션:
     * [0x00] [HDR=0x19] [payload...6bytes] [FTR=0x19] [HDR2=...] ...
     *         ↑헤더(4B)  ↑페이로드(16B)    ↑푸터(4B)
     *
     * 블록 크기 = 24 (헤더4 + 페이로드16 + 푸터4)
     * alloc = 1 (할당됨)
     * 헤더 값 = 24 | 1 = 25 = 0x19
     */
    unsigned int fake_heap[16] = {0};

    /* 헤더 설치 (배열[0] = 헤더) */
    fake_heap[0] = 24 | 1;   /* PACK(24, 1) = 0x19 */

    /* bp는 페이로드 시작 = 헤더 다음 주소 = &fake_heap[1] */
    char *bp = (char *)&fake_heap[1];

    /* 매크로 정의 */
    #define HDRP(bp)       ((char *)(bp) - WSIZE)
    #define GET(p)         (*(unsigned int *)(p))

    printf("bp(페이로드 시작) 주소:    %p\n", (void*)bp);

    /* TODO: HDRP(bp)를 이용해 헤더 주소를 출력하세요 */
    printf("HDRP(bp) 주소:             %p\n", (void*)(/* ??? */));

    /* TODO: 헤더에서 크기를 읽어 출력하세요 (GET + &~0x7 사용) */
    printf("헤더의 블록 크기:           %u (예상: 24)\n",
           GET(HDRP(bp)) & /* ??? */);

    /* TODO: 헤더에서 alloc 비트를 읽어 출력하세요 */
    printf("헤더의 alloc 비트:          %u (예상: 1)\n",
           GET(HDRP(bp)) & /* ??? */);

    /* -------------------------------------------------------
     * [문제 9-3] 다음 블록으로 이동하기
     *
     * NEXT_BLKP(bp) = (char*)bp + GET_SIZE(HDRP(bp))
     *
     * 현재 블록 크기가 24이므로:
     * 다음 블록 bp = 현재 bp + 24
     *
     * 왜 char*로 캐스팅?
     *   → 1바이트 단위로 정확하게 이동하기 위해
     *   → int*이면 4배 이동해버림!
     * ------------------------------------------------------- */
    printf("\n=== [문제 9-3] 다음 블록 포인터 계산 ===\n");

    /* 두 번째 블록 헤더 설치 (첫 블록 뒤 = fake_heap[6]) */
    /* 블록크기 24 = 6개 unsigned int (각 4바이트) */
    fake_heap[6] = 32 | 0;   /* PACK(32, 0) = 0x20: 크기32, free */

    /* TODO: NEXT_BLKP 계산: (char*)bp + (현재 블록 크기) */
    #define NEXT_BLKP(bp) ((char *)(bp) + (GET(HDRP(bp)) & ~0x7))

    char *next_bp = /* ??? */;

    printf("현재 bp:   %p\n", (void*)bp);
    printf("다음 bp:   %p\n", (void*)next_bp);
    printf("차이:      %ld바이트 (예상: 24)\n", next_bp - bp);

    /* 다음 블록 헤더 확인 */
    printf("다음 블록 크기:  %u (예상: 32)\n",
           GET(HDRP(next_bp)) & ~0x7);
    printf("다음 블록 alloc: %u (예상: 0, free)\n",
           GET(HDRP(next_bp)) & 0x1);

    /* -------------------------------------------------------
     * [문제 9-4] void* 캐스팅 실습
     *
     * mem_sbrk()가 void*를 반환할 때의 처리
     *
     * 실제 mm.c:
     *   void *bp = mem_sbrk(size);
     *   if (bp == (void *)-1) return NULL;  // 실패 체크
     *   *(unsigned int *)((char *)bp - WSIZE) = PACK(size, 1); // 헤더 쓰기
     * ------------------------------------------------------- */
    printf("\n=== [문제 9-4] void* 캐스팅 실습 ===\n");

    /* 힙 공간 시뮬레이션 */
    unsigned int sim_heap[8] = {0};
    void *heap_ptr = (void *)sim_heap;   /* mem_sbrk가 이걸 반환한다고 가정 */

    /*
     * heap_ptr을 받아서:
     * 1. (char *)heap_ptr + 0: 헤더 위치
     * 2. 헤더에 PACK(32, 0) 쓰기
     */

    /* TODO: heap_ptr을 char*로 캐스팅 후, 그 위치에 PACK(32, 0)을 PUT하세요 */
    /* 힌트: *(unsigned int *)((char *)heap_ptr + 0) = 32 | 0; */
    /* ??? */

    printf("sim_heap[0] = 0x%X (예상: 0x20)\n", sim_heap[0]);

    return 0;
}

/*
 * [컴파일 & 실행 방법]
 *   gcc -o 09_void 09_void_pointer.c
 *   ./09_void
 *
 * [mm-lab 연결고리]
 *   HDRP, FTRP, NEXT_BLKP, PREV_BLKP 매크로가
 *   전부 char* 캐스팅 + 바이트 단위 포인터 연산으로 만들어집니다.
 *   이걸 이해해야 mm_malloc의 힙 순회 로직을 짤 수 있습니다.
 *
 * [생각해보기]
 *   - (char*)bp - WSIZE 가 헤더 주소인 이유는?
 *   - int*로 캐스팅하면 왜 안 되나? (힌트: 4배 이동)
 *   - void*에 직접 +1 하면 GCC에서 경고가 나오는 이유?
 */
