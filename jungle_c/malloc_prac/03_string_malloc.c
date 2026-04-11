/*
 * ============================================================
 *  연습문제 03 - 문자열 동적 할당
 * ============================================================
 *
 * [핵심 개념]
 *
 * C의 문자열은 char 배열 + 끝에 '\0'(null terminator)입니다.
 *
 * Python:  s = "hello"     → 자동으로 메모리 관리
 * C:       char s[6] = "hello";  → 'h','e','l','l','o','\0' 필요
 *
 * 문자열을 동적으로 만들려면?
 *   char *s = (char *)malloc(sizeof(char) * (문자수 + 1));
 *                                              ↑ '\0' 공간 확보!
 *
 * [중요 함수들]
 *   strlen(s)         → 문자열 길이 반환 ('\0' 제외)
 *   strcpy(dst, src)  → src를 dst에 복사
 *   strcat(dst, src)  → dst 뒤에 src를 이어붙임
 *   strcmp(s1, s2)    → 두 문자열 비교 (같으면 0)
 *
 *   → 사용하려면 #include <string.h> 필요!
 *
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>   /* strlen, strcpy, strcat 사용 */

int main(void) {

    /* -------------------------------------------------------
     * [문제 3-1] 문자열을 저장할 공간 동적 할당
     *
     * "Hello, World!" 를 저장하려고 합니다.
     * 문자 수: 13개 + '\0' 1개 = 총 14바이트 필요
     *
     * strlen("Hello, World!") = 13 (자동으로 길이 계산)
     * malloc 크기 = strlen(...) + 1  ← +1이 '\0' 공간!
     * ------------------------------------------------------- */
    const char *original = "Hello, World!";
    int len = strlen(original);   /* = 13 */


    char *str = NULL;

    /* TODO: str에 len+1 크기의 char 배열을 동적 할당하세요 */
    str =  (char * )malloc(sizeof(char) * (len+1));

    if (str == NULL) {
        printf("메모리 할당 실패!\n");
        return 1;
    }

    /* -------------------------------------------------------
     * [문제 3-2] 문자열 복사하기
     *
     * str = original;  ← 이렇게 하면 안 됩니다!
     *   → 주소를 복사하는 것 (같은 메모리를 가리킴)
     *
     * strcpy(str, original);  ← 이게 맞습니다.
     *   → 내용(문자들)을 실제로 복사함
     * ------------------------------------------------------- */

    /* TODO: original의 내용을 str에 복사하세요 */
    strcpy(str, original);

    printf("복사된 문자열: %s\n", str);
    printf("문자열 길이: %d\n", (int)strlen(str));

    /* -------------------------------------------------------
     * [문제 3-3] 문자열 수정하기
     *
     * str은 포인터이므로 개별 문자 접근이 가능합니다.
     * str[0]  → 첫 번째 문자 'H'
     * str[7]  → 'W'
     *
     * 첫 번째 문자를 소문자 'h'로 바꿔보세요.
     * ------------------------------------------------------- */

    /* TODO: str의 첫 번째 문자를 'h'로 바꾸세요 */
    str[0] = 'h';

    printf("수정된 문자열: %s\n", str);

    /* -------------------------------------------------------
     * [문제 3-4] 두 문자열 이어붙이기
     *
     * " Bye!" 를 str 뒤에 붙이려고 합니다.
     * 현재 str의 공간은 14바이트 → 부족합니다!
     * 새로운 공간을 만들어야 합니다.
     *
     * 새 크기 = strlen(str) + strlen(" Bye!") + 1
     * ------------------------------------------------------- */
    const char *suffix = " Bye!";
    int new_len = strlen(str) + strlen(suffix);  /* +1은 아래서 */

    char *combined = NULL;

    /* TODO: combined에 new_len + 1 크기의 공간을 할당하세요 */
    combined = (char *)malloc (sizeof(char)*(new_len+1));

    if (combined == NULL) {
        free(str);
        printf("메모리 할당 실패!\n");
        return 1;
    }

    /* TODO: str을 combined에 복사하고, suffix를 이어붙이세요 */
    /* 힌트: strcpy 후 strcat 사용 */
    strcpy(combined, str);
    strcat(combined, suffix);

    printf("이어붙인 문자열: %s\n", combined);

    /* -------------------------------------------------------
     * [문제 3-5] 모든 동적 할당 메모리 해제
     *
     * str과 combined 둘 다 해제해야 합니다.
     * 순서는 상관없지만, 빠뜨리면 메모리 누수!
     * ------------------------------------------------------- */

    /* TODO: str과 combined를 free하고 NULL로 초기화하세요 */
    free(str);
    free(combined);
    str = NULL;
    combined = NULL;

    printf("메모리 해제 완료\n");
    return 0;
}

/*
 * [컴파일 & 실행 방법]
 *   gcc -o 03_string 03_string_malloc.c
 *   ./03_string
 *
 * [예상 출력]
 *   복사된 문자열: Hello, World!
 *   문자열 길이: 13
 *   수정된 문자열: hello, World!
 *   이어붙인 문자열: hello, World! Bye!
 *   메모리 해제 완료
 *
 * [생각해보기]
 *   - str = original 처럼 주소를 그냥 대입하면 무슨 문제가 생길까?
 *   - '\0' 없이 printf("%s") 하면 어떻게 될까?
 */
