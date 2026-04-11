/*
 * ============================================================
 *  연습문제 06 - 연결 리스트 노드 malloc/free
 * ============================================================
 *
 * [핵심 개념]
 *
 * 연결 리스트의 각 노드는 힙에 개별적으로 할당됩니다.
 *
 * Python:
 *   class Node:
 *       def __init__(self, data):
 *           self.data = data
 *           self.next = None
 *
 * C:
 *   typedef struct Node {
 *       int data;
 *       struct Node *next;   ← 자기 자신 타입의 포인터!
 *   } Node;
 *
 * [노드 생성]
 *   Node *node = (Node *)malloc(sizeof(Node));
 *   node->data = 10;
 *   node->next = NULL;
 *
 * [연결 구조]
 *   head → [10|*] → [20|*] → [30|NULL]
 *           node1    node2    node3
 *
 * [free 순서]
 *   head부터 순서대로 free해야 합니다.
 *   반드시 next 주소를 임시 저장 후 free!
 *
 *   Node *cur = head;
 *   while (cur != NULL) {
 *       Node *next = cur->next;   ← 먼저 저장!
 *       free(cur);                ← 그 다음 free
 *       cur = next;               ← 저장한 주소로 이동
 *   }
 *
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>

/* 노드 구조체 */
typedef struct Node {
    int data;
    struct Node *next;   /* 다음 노드의 주소. 없으면 NULL */
} Node;

/* -------------------------------------------------------
 * 새 노드를 생성하여 반환하는 함수
 *
 * [흐름]
 *   1. malloc으로 Node 하나 할당
 *   2. data 초기화
 *   3. next를 NULL로 초기화  ← 항상 NULL로 시작!
 *   4. 포인터 반환
 * ------------------------------------------------------- */
Node *create_node(int data) {

    /* TODO: Node 하나를 malloc으로 할당하세요 */
    Node *node = /* ??? */;

    if (node == NULL) {
        printf("노드 생성 실패!\n");
        return NULL;
    }

    /* TODO: node->data와 node->next를 초기화하세요 */
    /* ??? */
    node->next = NULL;

    return node;
}

/* -------------------------------------------------------
 * 맨 앞에 노드를 추가하는 함수 (prepend)
 *
 * [흐름]
 *   새 노드 → [기존 head] 로 연결
 *
 *   before: head → [10] → [20] → NULL
 *   after:  head → [5] → [10] → [20] → NULL
 *
 * *head를 수정해야 하므로 Node **head (이중 포인터) 사용
 * Python 비유: head 변수 자체를 바꾸는 것 = 이중 포인터 필요
 * ------------------------------------------------------- */
void prepend(Node **head, int data) {
    Node *node = create_node(data);
    if (node == NULL) return;

    /* TODO: node->next가 현재 head를 가리키게 하세요 */
    node->next = /* ??? */;

    /* TODO: head를 새 노드로 업데이트하세요 */
    *head = /* ??? */;
}

/* -------------------------------------------------------
 * 맨 뒤에 노드를 추가하는 함수 (append)
 *
 * [흐름]
 *   1. head가 NULL이면 새 노드가 head
 *   2. 아니면 마지막 노드까지 이동
 *   3. 마지막 노드의 next를 새 노드로 연결
 *
 *   before: head → [10] → [20] → NULL
 *   after:  head → [10] → [20] → [30] → NULL
 * ------------------------------------------------------- */
void append(Node **head, int data) {
    Node *node = create_node(data);
    if (node == NULL) return;

    /* 빈 리스트인 경우 */
    if (*head == NULL) {
        *head = node;
        return;
    }

    /* 마지막 노드까지 이동 */
    Node *cur = *head;

    /* TODO: cur->next != NULL인 동안 cur를 다음으로 이동하세요 */
    while (/* ??? */) {
        /* ??? */;
    }

    /* TODO: 마지막 노드의 next를 새 노드로 연결하세요 */
    /* ??? */
}

/* -------------------------------------------------------
 * 리스트 전체를 출력하는 함수
 * ------------------------------------------------------- */
void print_list(Node *head) {
    Node *cur = head;
    printf("head → ");

    /* TODO: cur가 NULL이 아닌 동안 data를 출력하며 이동하세요 */
    while (/* ??? */) {
        printf("[%d] → ", /* ??? */);
        /* ??? */;
    }
    printf("NULL\n");
}

/* -------------------------------------------------------
 * 특정 값을 가진 노드를 삭제하는 함수
 *
 * [주의] 삭제 시 앞 노드의 next를 연결해야 함
 *
 * case 1: 삭제할 노드가 head인 경우
 *   head → [10] → [20] → NULL
 *   head = head->next   (head를 다음 노드로 변경)
 *   free(old_head)
 *
 * case 2: 중간/끝 노드 삭제
 *   prev → [target] → next
 *   prev->next = target->next
 *   free(target)
 * ------------------------------------------------------- */
void delete_node(Node **head, int target) {

    if (*head == NULL) return;

    /* case 1: head가 삭제 대상 */
    if ((*head)->data == target) {
        Node *to_free = *head;
        /* TODO: head를 다음 노드로 업데이트하세요 */
        *head = /* ??? */;
        free(to_free);
        return;
    }

    /* case 2: 중간/끝 노드 탐색 */
    Node *prev = *head;
    Node *cur  = (*head)->next;

    while (cur != NULL) {
        if (cur->data == target) {
            /* TODO: prev->next를 cur->next로 연결하세요 */
            /* ??? */;
            free(cur);
            return;
        }
        /* TODO: prev와 cur를 한 칸씩 이동하세요 */
        prev = /* ??? */;
        cur  = /* ??? */;
    }

    printf("%d를 찾을 수 없습니다.\n", target);
}

/* -------------------------------------------------------
 * 리스트 전체를 해제하는 함수
 *
 * [핵심] free하기 전에 next를 반드시 저장해야 함!
 * ------------------------------------------------------- */
void free_list(Node **head) {
    Node *cur = *head;

    while (cur != NULL) {
        /* TODO: 다음 노드 주소를 임시 저장하세요 */
        Node *next = /* ??? */;

        /* TODO: 현재 노드를 free하세요 */
        /* ??? */;

        /* TODO: cur를 다음 노드로 이동하세요 */
        cur = /* ??? */;
    }

    *head = NULL;
}

int main(void) {

    Node *head = NULL;

    /* 뒤에 추가 */
    printf("=== append 테스트 ===\n");
    append(&head, 10);
    append(&head, 20);
    append(&head, 30);
    print_list(head);   /* 예상: head → [10] → [20] → [30] → NULL */

    /* 앞에 추가 */
    printf("=== prepend 테스트 ===\n");
    prepend(&head, 5);
    print_list(head);   /* 예상: head → [5] → [10] → [20] → [30] → NULL */

    /* 노드 삭제 */
    printf("=== delete 테스트 ===\n");
    delete_node(&head, 20);
    print_list(head);   /* 예상: head → [5] → [10] → [30] → NULL */

    delete_node(&head, 5);
    print_list(head);   /* 예상: head → [10] → [30] → NULL */

    /* 전체 해제 */
    printf("=== free_list ===\n");
    free_list(&head);
    print_list(head);   /* 예상: head → NULL */

    return 0;
}

/*
 * [컴파일 & 실행 방법]
 *   gcc -o 06_list 06_linked_list.c
 *   ./06_list
 *
 * [생각해보기]
 *   - free(cur) 전에 Node *next = cur->next 가 없으면 어떻게 될까?
 *   - cur->next = cur 처럼 자기 자신을 가리키면 무한 루프?
 *   - delete_node에서 없는 값(예: 99)을 삭제하려 하면?
 */
