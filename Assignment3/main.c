#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_TREE 2048
#define MAX_STACK 512

typedef struct {
    int data[MAX_STACK];
    int top;
} IntStack;

void initStack(IntStack* s) { s->top = -1; }
int isEmpty(IntStack* s) { return s->top == -1; }
void push(IntStack* s, int v) { s->data[++s->top] = v; }
int pop(IntStack* s) { return isEmpty(s) ? -1 : s->data[s->top--]; }
int peek(IntStack* s) { return isEmpty(s) ? -1 : s->data[s->top]; }

/* 괄호표기 → 배열 트리 변환 */
int parseToHeap(const char* in, char tree[], int size) {
    IntStack st; initStack(&st);
    int lastIdx = 0;
    int childFlag = 0; // 0=왼쪽, 1=오른쪽

    for (int i = 0; in[i]; ++i) {
        char c = in[i];
        if (isspace((unsigned char)c)) continue;

        if (c == '(') {
            if (lastIdx != 0) {
                push(&st, lastIdx);
                childFlag = 0;
            }
            lastIdx = 0;
        }
        else if (c == ')') {
            pop(&st);
            lastIdx = 0;
        }
        else if (isupper((unsigned char)c)) {
            int idx;
            if (isEmpty(&st)) idx = 1; // root
            else {
                int parent = peek(&st);
                idx = (childFlag == 0) ? parent * 2 : parent * 2 + 1;
            }
            if (idx >= size) {
                fprintf(stderr, "배열 초과!\n");
                return -1;
            }
            tree[idx] = c;
            lastIdx = idx;
            childFlag = 1; // 같은 부모 밑의 다음 노드는 오른쪽
        }
    }
    return 0;
}

///* --- Traversals --- */
//void preorder(char tree[], int idx, int size) {
//    if (idx >= size || tree[idx] == 0) return;
//    printf("%c ", tree[idx]);
//    preorder(tree, idx * 2, size);     // left
//    preorder(tree, idx * 2 + 1, size);   // right
//}
//
//void inorder(char tree[], int idx, int size) {
//    if (idx >= size || tree[idx] == 0) return;
//    inorder(tree, idx * 2, size);      // left
//    printf("%c ", tree[idx]);
//    inorder(tree, idx * 2 + 1, size);    // right
//}
//
//void postorder(char tree[], int idx, int size) {
//    if (idx >= size || tree[idx] == 0) return;
//    postorder(tree, idx * 2, size);    // left
//    postorder(tree, idx * 2 + 1, size);  // right
//    printf("%c ", tree[idx]);
//}

/* 전위 순회: root → left → right */
void preorder(char tree[], int root, int size) {
    IntStack st; initStack(&st);
    if (tree[root] == 0) return;

    push(&st, root);
    while (!isEmpty(&st)) {
        int idx = pop(&st);
        printf("%c ", tree[idx]);

        if (idx * 2 + 1 < size && tree[idx * 2 + 1]) push(&st, idx * 2 + 1); // right
        if (idx * 2 < size && tree[idx * 2]) push(&st, idx * 2);             // left
    }
}

/* 중위 순회: left → root → right */
void inorder(char tree[], int root, int size) {
    IntStack st; initStack(&st);
    int curr = root;

    while (curr < size && curr > 0 || !isEmpty(&st)) {
        while (curr < size && curr > 0 && tree[curr]) {
            push(&st, curr);
            curr = curr * 2; // 왼쪽으로
        }
        if (isEmpty(&st)) break;

        curr = pop(&st);
        printf("%c ", tree[curr]);

        curr = curr * 2 + 1; // 오른쪽으로
    }
}

/* 후위 순회: left → right → root */
void postorder(char tree[], int root, int size) {
    if (tree[root] == 0) return;
    IntStack s1, s2;
    initStack(&s1); initStack(&s2);

    push(&s1, root);
    while (!isEmpty(&s1)) {
        int idx = pop(&s1);
        push(&s2, idx);

        if (idx * 2 < size && tree[idx * 2]) push(&s1, idx * 2);         // left
        if (idx * 2 + 1 < size && tree[idx * 2 + 1]) push(&s1, idx * 2 + 1); // right
    }
    while (!isEmpty(&s2)) {
        printf("%c ", tree[pop(&s2)]);
    }
}

int main(void) {
    static char tree[MAX_TREE] = { 0 };
    char input[1024];

    printf("괄호표기 트리 입력: ");
    fgets(input, sizeof(input), stdin);

    if (parseToHeap(input, tree, MAX_TREE) == 0) {
        for (int i = 1; i < 512; ++i) {
            if (tree[i]) {
                printf("[%d] = %c\n", i, tree[i]);
            }
        }

        printf("\npre-order: ");
        preorder(tree, 1, MAX_TREE);

        printf("\nin-order: ");
        inorder(tree, 1, MAX_TREE);

        printf("\npost-order: ");
        postorder(tree, 1, MAX_TREE);

        printf("\n");
    }
    return 0;
}
