#include <stdio.h>
#include <stdlib.h>

typedef struct TreeNode {
    char data;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

// 새로운 노드 생성
TreeNode* create_node(char key) {
    TreeNode* newNode = (TreeNode*)malloc(sizeof(TreeNode));
    if (!newNode) {
        printf("메모리 할당 실패\n");
        exit(1);
    }
    newNode->data = key;
    newNode->left = newNode->right = NULL;
    return newNode;
}

// 노드 삽입
TreeNode* insert_node(TreeNode* root, TreeNode* parent, char key, char child) {
    TreeNode* newNode = create_node(key);

    // 트리가 비어있을 경우, root 생성
    if (root == NULL) {
        if (parent != NULL) {
            printf("Root가 없는데 parent가 주어졌습니다.\n");
            free(newNode);
            return root;
        }
        return newNode; // 첫 root 반환
    }

    if (parent == NULL) {
        printf("Parent가 NULL일 수 없습니다.\n");
        free(newNode);
        return root;
    }

    if (child == 'L') {
        if (parent->left == NULL)
            parent->left = newNode;
        else {
            printf("왼쪽 자식이 이미 존재합니다.\n");
            free(newNode);
        }
    }
    else if (child == 'R') {
        if (parent->right == NULL)
            parent->right = newNode;
        else {
            printf("오른쪽 자식이 이미 존재합니다.\n");
            free(newNode);
        }
    }
    else {
        printf("child는 'L' 또는 'R'이어야 합니다.\n");
        free(newNode);
    }

    return root;
}

// 노드 삭제
TreeNode* delete_node(TreeNode* root, TreeNode* parent, char child) {
    if (root == NULL || parent == NULL) return root;

    TreeNode* target = NULL;

    if (child == 'L') target = parent->left;
    else if (child == 'R') target = parent->right;

    if (target == NULL) {
        printf("삭제할 노드가 존재하지 않습니다.\n");
        return root;
    }

    // 단말 노드만 삭제 허용
    if (target->left == NULL && target->right == NULL) {
        if (child == 'L') parent->left = NULL;
        else parent->right = NULL;
        free(target);
    }
    else {
        printf("삭제하려는 노드가 단말 노드가 아닙니다.\n");
    }

    return root;
}

// 트리 출력 (전위순회)
void preorder(TreeNode* root) {
    if (root != NULL) {
        printf("%c ", root->data);
        preorder(root->left);
        preorder(root->right);
    }
}

// 메모리 해제
void free_tree(TreeNode* root) {
    if (root != NULL) {
        free_tree(root->left);
        free_tree(root->right);
        free(root);
    }
}

// 테스트용 main
int main() {
    TreeNode* root = NULL;

    // Root 생성
    root = insert_node(root, NULL, 'A', 'L');

    // A의 왼쪽 자식 B, 오른쪽 자식 C 추가
    // Root, Parent, Data, Direction
    root = insert_node(root, root, 'B', 'L');
    root = insert_node(root, root, 'C', 'R');

    // B의 왼쪽 자식 D 추가
    root = insert_node(root, root->left, 'D', 'L');

    printf("전위순회: ");
    preorder(root);
    printf("\n");

    // 노드 삭제 (D 삭제)
    root = delete_node(root, root->left, 'L');

    printf("삭제 후 전위순회: ");
    preorder(root);
    printf("\n");

    free_tree(root);
    return 0;
}
