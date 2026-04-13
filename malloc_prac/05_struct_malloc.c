/*
 * ============================================================
 *  연습문제 05 - 구조체 동적 할당
 * ============================================================
 *
 * [핵심 개념]
 *
 * Python의 클래스:
 *   class Student:
 *       def __init__(self, name, age, score):
 *           self.name = name
 *           self.age = age
 *           self.score = score
 *
 * C의 구조체 (struct):
 *   struct Student {
 *       char name[50];
 *       int  age;
 *       float score;
 *   };
 *
 * 구조체를 동적 할당하면?
 *   struct Student *s = (struct Student *)malloc(sizeof(struct Student));
 *
 * [멤버 접근 방법]
 *   구조체 변수:    s.name    (점 연산자)
 *   구조체 포인터:  s->name   (화살표 연산자)  ←→  (*s).name
 *
 *   s->name  === (*s).name   ← 완전히 동일한 표현!
 *
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 학생 구조체 정의 */
typedef struct {
    char  name[50];   /* 이름 (최대 49자 + '\0') */
    int   age;        /* 나이 */
    float score;      /* 점수 */
} Student;

/*
 * 학생 정보를 출력하는 함수
 *
 * 포인터로 받는 이유:
 *   - 값으로 전달(Student s)하면 구조체 전체가 복사됨 → 비효율
 *   - 포인터로 전달(Student *s)하면 주소(8바이트)만 전달 → 효율적
 *   - const를 붙이면 함수 내에서 수정 불가 → 안전
 */
void print_student(const Student *s) {
    /* TODO: s->name, s->age, s->score를 출력하세요 */
    printf("이름: %s, 나이: %d, 점수: %.1f\n",
           s -> name, s -> age , s -> score);
}

/*
 * 학생 구조체를 동적으로 생성하고 초기화하는 함수
 * 생성된 구조체의 포인터를 반환합니다.
 *
 * 반환 타입이 Student* 인 이유:
 *   함수 내에서 만든 포인터를 밖으로 전달하기 위함
 *   (스택 변수의 주소를 반환하면 위험! 힙은 괜찮음)
 */
Student *create_student(const char *name, int age, float score) {

    /* TODO: Student 하나를 저장할 공간을 malloc으로 할당하세요 */
    Student *s = (Student*)malloc(sizeof(Student));

    if (s == NULL) {
        printf("메모리 할당 실패!\n");
        return NULL;
    }

    /* TODO: s->name에 name을 복사하세요 (strcpy 사용) */
    strcpy(s->name, name);

    /* TODO: s->age와 s->score에 값을 저장하세요 */
    s -> age = age;
    s -> score = score;

    return s;
}

int main(void) {

    /* -------------------------------------------------------
     * [문제 5-1] 학생 단일 동적 할당
     *
     * create_student 함수를 사용해서 학생 3명을 만드세요.
     * ------------------------------------------------------- */
    Student *s1 = create_student("Alice", 20, 92.5f);
    Student *s2 = create_student("Bob",   22, 88.0f);
    Student *s3 = create_student("Carol", 21, 95.5f);

    printf("=== 학생 정보 ===\n");
    print_student(s1);
    print_student(s2);
    print_student(s3);

    /* -------------------------------------------------------
     * [문제 5-2] 점수 수정하기
     *
     * s2의 점수를 90.0으로 수정하세요.
     * 포인터이므로 -> 연산자를 사용합니다.
     * ------------------------------------------------------- */

    /* TODO: s2->score를 90.0f로 변경하세요 */
    s2 -> score = 90.0f;

    printf("\n=== Bob 점수 수정 후 ===\n");
    print_student(s2);

    /* -------------------------------------------------------
     * [문제 5-3] 학생 배열 동적 할당
     *
     * 학생 5명을 배열로 관리하고 싶습니다.
     *
     * 방법:  Student *arr = malloc(sizeof(Student) * 5);
     * 접근:  arr[i].name   (포인터지만 배열처럼 접근)
     *   또는 (arr + i)->name  (포인터 연산)
     * ------------------------------------------------------- */
    int count = 5;
    Student *class = NULL;

    /* TODO: Student count개를 저장할 배열을 동적 할당하세요 */
    class = malloc(sizeof(Student)* count);

    if (class == NULL) {
        free(s1); free(s2); free(s3);
        return 1;
    }

    /* 학생 배열에 데이터 채우기 */
    const char *names[] = {"Dave", "Eve", "Frank", "Grace", "Henry"};
    int ages[]           = {19, 23, 20, 21, 22};
    float scores[]       = {78.0f, 85.5f, 91.0f, 76.5f, 88.0f};

    for (int i = 0; i < count; i++) {
        /* TODO: class[i].name에 names[i]를 복사하세요 */
        strcpy(class[i].name, names[i]);

        /* TODO: class[i].age와 class[i].score에 값을 저장하세요 */
        class[i].age = ages[i];
        class[i].score = scores[i];
    }

    /* 배열 출력 */
    printf("\n=== 학급 전체 학생 ===\n");
    for (int i = 0; i < count; i++) {
        /* TODO: 아래 NULL을 &class[i]로 바꾸세요 */
        /* 힌트: class[i]는 구조체 값 → 주소를 넘기려면 &class[i] */
        print_student(&class[i]);   /* ← 여기를 수정! */
    }

    /* -------------------------------------------------------
     * [문제 5-4] 최고 점수 학생 찾기
     * ------------------------------------------------------- */
    int best_idx = 0;
    for (int i = 1; i < count; i++) {
        /* TODO: class[i].score > class[best_idx].score 이면 best_idx = i */
        if(class[i].score > class[best_idx].score)
        {
            best_idx = i;
        }
    }
    printf("\n최고 점수 학생: %s (%.1f점)\n",
           class[best_idx].name, class[best_idx].score);

    /* -------------------------------------------------------
     * [문제 5-5] 모든 동적 메모리 해제
     * ------------------------------------------------------- */

    /* TODO: s1, s2, s3 각각 free하고 NULL 초기화 */
    free(s1);
    free(s2);
    free(s3);
    s1 = NULL;
    s2 = NULL;
    s3 = NULL;

    /* TODO: class 배열을 free하고 NULL 초기화 */
    free(class);
    class = NULL;

    printf("\n메모리 해제 완료\n");
    return 0;
}

/*
 * [컴파일 & 실행 방법]
 *   gcc -o 05_struct 05_struct_malloc.c
 *   ./05_struct
 *
 * [예상 출력]
 *   === 학생 정보 ===
 *   이름: Alice, 나이: 20, 점수: 92.5
 *   이름: Bob, 나이: 22, 점수: 88.0
 *   이름: Carol, 나이: 21, 점수: 95.5
 *
 *   === Bob 점수 수정 후 ===
 *   이름: Bob, 나이: 22, 점수: 90.0
 *   ...
 *   최고 점수 학생: Frank (91.0점)
 *
 * [생각해보기]
 *   - s->score 와 (*s).score 는 왜 동일한가?
 *   - class 배열의 각 요소(Student)는 따로 free해야 할까?
 *     (malloc 한 번 vs malloc 여러 번의 차이)
 */
