/*
 * ============================================================
 *  연습문제 07 - realloc (크기 재조정)
 * ============================================================
 *
 * [핵심 개념]
 *
 * Python 리스트는 공간이 부족하면 알아서 늘어납니다.
 *   arr = []
 *   arr.append(1)  # 내부적으로 realloc 같은 동작
 *
 * C에서 이미 할당한 메모리 크기를 바꾸려면 realloc을 씁니다.
 *
 *   void *realloc(void *ptr, size_t new_size);
 *
 * [realloc의 동작]
 *   1. 현재 메모리 블록을 new_size로 늘리거나 줄임
 *   2. 공간이 충분하면 그 자리에서 확장 (같은 주소 반환)
 *   3. 공간이 부족하면 새 위치로 이사 후 기존 데이터 복사
 *      → 새 주소 반환, 기존 메모리는 자동으로 free됨
 *   4. 실패 시 NULL 반환 (기존 메모리는 그대로!)
 *
 * [중요한 실수 패턴]
 *   ptr = realloc(ptr, new_size);  ← 위험!
 *   → 실패 시 ptr이 NULL이 되어 기존 메모리 주소를 잃음
 *
 *   올바른 패턴:
 *   void *tmp = realloc(ptr, new_size);
 *   if (tmp == NULL) {
 *       // ptr은 여전히 유효 → 처리 후 free 가능
 *       free(ptr);
 *       return;
 *   }
 *   ptr = tmp;   ← 성공 시에만 ptr 업데이트
 *
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * Python의 list.append()처럼 동작하는 동적 배열 구조체
 * capacity만큼 공간을 미리 잡고, size가 꽉 차면 2배로 늘림
 */
typedef struct {
    int *data;      /* 실제 데이터가 저장된 힙 공간 */
    int  size;      /* 현재 저장된 원소 수 */
    int  capacity;  /* 현재 할당된 총 공간 크기 */
} DynArray;

/* -------------------------------------------------------
 * DynArray 초기화 함수
 * 초기 capacity만큼 공간을 할당합니다.
 * ------------------------------------------------------- */
DynArray *create_dynarray(int init_capacity) {

    /* TODO: DynArray 구조체 자체를 malloc으로 할당하세요 */
    DynArray *arr = /* ??? */;
    if (arr == NULL) return NULL;

    /* TODO: arr->data에 init_capacity개의 int 공간을 할당하세요 */
    arr->data = /* ??? */;
    if (arr->data == NULL) {
        free(arr);
        return NULL;
    }

    /* TODO: arr->size = 0, arr->capacity = init_capacity로 초기화 */
    /* ??? */
    /* ??? */

    return arr;
}

/* -------------------------------------------------------
 * 배열에 원소를 추가하는 함수 (Python의 list.append)
 *
 * size == capacity 이면 capacity를 2배로 늘립니다.
 * ------------------------------------------------------- */
void push_back(DynArray *arr, int value) {

    /* 공간이 꽉 찼으면 2배로 확장 */
    if (arr->size == arr->capacity) {
        int new_cap = arr->capacity * 2;

        /* TODO: realloc으로 arr->data를 new_cap 크기로 늘리세요 */
        /* 안전한 패턴: tmp를 먼저 받고, NULL 체크 후 arr->data에 대입 */
        int *tmp = /* ??? */;

        if (tmp == NULL) {
            printf("realloc 실패!\n");
            return;
        }

        /* TODO: arr->data를 tmp로 업데이트하고 capacity도 갱신 */
        /* ??? */
        /* ??? */

        printf("  [resize] capacity: %d → %d\n",
               arr->capacity / 2, arr->capacity);   /* 이미 갱신됨 */
    }

    /* TODO: arr->data[arr->size]에 value를 저장하고 size를 1 증가 */
    /* ??? */
    /* ??? */
}

/* -------------------------------------------------------
 * 배열 출력 함수
 * ------------------------------------------------------- */
void print_dynarray(const DynArray *arr) {
    printf("size=%d, capacity=%d, data=[", arr->size, arr->capacity);
    for (int i = 0; i < arr->size; i++) {
        printf("%d", arr->data[i]);
        if (i < arr->size - 1) printf(", ");
    }
    printf("]\n");
}

/* -------------------------------------------------------
 * DynArray 해제 함수
 * 내부 data 먼저 free → 구조체 free 순서!
 * ------------------------------------------------------- */
void free_dynarray(DynArray **arr) {
    if (*arr == NULL) return;

    /* TODO: (*arr)->data를 free하세요 */
    /* ??? */

    /* TODO: 구조체 *arr를 free하고 NULL로 초기화하세요 */
    /* ??? */
    /* ??? */
}

/* -------------------------------------------------------
 * [문제 7-2] realloc으로 문자열 버퍼 확장하기
 *
 * 여러 문자열을 하나로 이어붙이는데,
 * 크기가 부족할 때마다 realloc으로 늘립니다.
 * ------------------------------------------------------- */
char *build_string(const char *words[], int count) {

    int buf_size = 32;           /* 초기 버퍼 크기 */
    int cur_len  = 0;

    /* TODO: buf_size 크기의 char 배열을 malloc으로 할당하세요 */
    char *buf = /* ??? */;
    if (buf == NULL) return NULL;
    buf[0] = '\0';   /* 빈 문자열로 초기화 */

    for (int i = 0; i < count; i++) {
        /* 필요한 총 크기 계산: 현재 길이 + 추가 단어 + 공백 + '\0' */
        int word_len  = 0;
        while (words[i][word_len] != '\0') word_len++;  /* strlen 직접 구현 */
        int needed = cur_len + word_len + 2;  /* +1 공백, +1 '\0' */

        /* 공간 부족하면 realloc */
        if (needed > buf_size) {
            while (buf_size < needed) buf_size *= 2;

            /* TODO: buf를 buf_size로 realloc하세요 (안전한 패턴 사용) */
            char *tmp = /* ??? */;
            if (tmp == NULL) {
                free(buf);
                return NULL;
            }
            /* ??? */
            printf("  [buf resize] → %d bytes\n", buf_size);
        }

        /* 단어 이어붙이기 */
        if (cur_len > 0) {
            buf[cur_len++] = ' ';   /* 단어 사이 공백 */
        }
        for (int j = 0; words[i][j] != '\0'; j++) {
            buf[cur_len++] = words[i][j];
        }
        buf[cur_len] = '\0';
    }

    return buf;
}

int main(void) {

    /* -------------------------------------------------------
     * 테스트 1: DynArray (Python list.append 시뮬레이션)
     * ------------------------------------------------------- */
    printf("=== DynArray 테스트 ===\n");
    DynArray *da = create_dynarray(2);   /* 초기 capacity=2 */

    for (int i = 1; i <= 8; i++) {
        push_back(da, i * 10);
        print_dynarray(da);
    }

    free_dynarray(&da);
    printf("da 해제 완료, da=%p\n\n", (void*)da);

    /* -------------------------------------------------------
     * 테스트 2: 문자열 버퍼 확장
     * ------------------------------------------------------- */
    printf("=== 문자열 이어붙이기 ===\n");
    const char *words[] = {"Hello", "World", "from", "dynamic", "memory", "allocation"};
    int word_count = 6;

    char *result = build_string(words, word_count);
    if (result) {
        printf("결과: \"%s\"\n", result);
        /* TODO: result를 free하세요 */
        /* ??? */
    }

    return 0;
}

/*
 * [컴파일 & 실행 방법]
 *   gcc -o 07_realloc 07_realloc.c
 *   ./07_realloc
 *
 * [예상 출력 (일부)]
 *   === DynArray 테스트 ===
 *   size=1, capacity=2, data=[10]
 *   size=2, capacity=2, data=[10, 20]
 *     [resize] capacity: 2 → 4
 *   size=3, capacity=4, data=[10, 20, 30]
 *   ...
 *
 * [생각해보기]
 *   - ptr = realloc(ptr, size) 패턴의 문제점은?
 *   - capacity를 2배씩 늘리는 이유는? (1씩 늘리면 안 되나?)
 *   - realloc(ptr, 0) 은 어떻게 동작할까? (free와 같을까?)
 */
