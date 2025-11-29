#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int id;
	char name[50];
	char gender;
	int korean_grade;
	int english_grade;
	int math_grade;
	int sum_grade;
} Student;

Student* load_students_info() {
	FILE* csv = fopen("student.csv", "r");
	if (!csv) {
		fprintf(stderr, "student.csv 탐색 불가");
		return NULL;
	}

	char line[]
}

int bubble_sort(Student* st);
int select_sort(Student* st);
int insert_sort(Student* st);
int shell_sort(Student* st);
int quick_sort(Student* st);
int merge_sort(Student* st);
int radix_sort(Student* st);
int tree_sort(Student* st);
int heap_sort(Student* st);

int main() {
	Student st_arr[33000] = load_students_info();
	

	return 0;
}