#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BUCKETS 10
#define SLOTS_PER_BUCKET 10
#define TABLE_SIZE (BUCKETS * SLOTS_PER_BUCKET)
#define EMPTY -1

typedef enum { MID_SQUARE = 0, REMAINDER = 1, FOLDING = 2 } HashType;
typedef enum { LINEAR_PROBING = 0, DOUBLE_HASHING = 1 } ResolveType;

typedef struct {
    int collisions;               // 해시 함수 충돌 횟수 (첫 슬롯이 이미 차 있는 경우)
    int overflow_comparisons;     // 오버플로우(재조사) 중 슬롯 비교 횟수 (삽입 기준)
    int search_comparisons;       // 검색 시 비교 횟수 합
} Stats;

// 0~1000 범위의 key에 대해 중간제곱 해시: key^2의 오른쪽에서 4번째 자리 숫자 사용 (0~9)
int mid_square_bucket(int key) {
    long sq = (long)key * (long)key; // 최대 1,000,000
    int digit4 = (sq / 1000) % 10;   // 오른쪽에서 4번째 자리
    return digit4;                   // 0~9
}

// 나머지 해시: 10으로 나눈 나머지 (0~9)
int remainder_bucket(int key) {
    return key % 10;
}

// 접지 해시: 자릿수를 모두 더한 후 10으로 나눈 값 (0~9)
int folding_bucket(int key) {
    int sum = 0;
    int x = key;
    if (x == 0) return 0;
    while (x > 0) {
        sum += x % 10;
        x /= 10;
    }
    return sum % 10;
}

// 실제 테이블 인덱스(0~99)로 변환
// 각 해시 함수가 bucket(0~9)를 결정하고, slot(0~9)는 키의 다른 정보로 구성
int hash_index(int key, HashType type) {
    int bucket;
    int slot;
    switch (type) {
    case MID_SQUARE:
        bucket = mid_square_bucket(key);   // 0~9
        slot = key % SLOTS_PER_BUCKET;   // 0~9
        break;
    case REMAINDER:
        bucket = remainder_bucket(key);    // 0~9
        slot = (key / 10) % SLOTS_PER_BUCKET; // 0~9 (십의 자리)
        break;
    case FOLDING:
    default:
        bucket = folding_bucket(key);      // 0~9
        slot = key % SLOTS_PER_BUCKET;   // 0~9
        break;
    }
    return bucket * SLOTS_PER_BUCKET + slot; // 0~99
}

// 2차 해시에서 사용할 보조 해시 함수 (step 크기)
// 7 - (key % 7) => 1~7, 100과 서로소(1,3,7)들이 나와 전체 테이블을 잘 순회
int second_hash_step(int key) {
    int step = 7 - (key % 7);
    if (step == 0) step = 1; // 이론상 안 나오지만 안전하게
    return step;
}

void init_table(int table[]) {
    for (int i = 0; i < TABLE_SIZE; ++i) {
        table[i] = EMPTY;
    }
}

// key 삽입 (오픈 어드레싱), 성공 시 1, 실패 시 0
int insert_key(int table[], int key, HashType htype, ResolveType rtype, Stats* st) {
    int base = hash_index(key, htype);
    int step = 1;
    if (rtype == DOUBLE_HASHING) {
        step = second_hash_step(key);
    }

    for (int i = 0; i < TABLE_SIZE; ++i) {
        int pos;
        if (rtype == LINEAR_PROBING) {
            pos = (base + i) % TABLE_SIZE;
        }
        else { // DOUBLE_HASHING
            pos = (base + i * step) % TABLE_SIZE;
        }

        if (i > 0) {
            // 오버플로우로 인해 다시 조사하는 모든 슬롯 검사
            st->overflow_comparisons++;
        }

        if (table[pos] == EMPTY) {
            table[pos] = key;
            return 1;
        }
        else if (table[pos] == key) {
            // 이미 존재하는 키 -> 삽입 안 함 (하지만 비교는 했음)
            return 0;
        }
        else {
            // 다른 키가 들어있는 경우 충돌
            if (i == 0) {
                st->collisions++;
            }
            // 다음 위치로 진행
        }
    }

    // 테이블이 가득 차서 삽입 실패
    return 0;
}

// key 검색, 찾으면 1, 못 찾으면 0
int search_key(int table[], int key, HashType htype, ResolveType rtype, Stats* st) {
    int base = hash_index(key, htype);
    int step = 1;
    if (rtype == DOUBLE_HASHING) {
        step = second_hash_step(key);
    }

    for (int i = 0; i < TABLE_SIZE; ++i) {
        int pos;
        if (rtype == LINEAR_PROBING) {
            pos = (base + i) % TABLE_SIZE;
        }
        else {
            pos = (base + i * step) % TABLE_SIZE;
        }

        st->search_comparisons++;

        if (table[pos] == EMPTY) {
            // 비어 있는 슬롯을 만나면 더 이상 진행할 필요 없음
            return 0;
        }
        if (table[pos] == key) {
            return 1;
        }
    }

    return 0;
}

const char* hash_name(HashType h) {
    switch (h) {
    case MID_SQUARE: return "중간제곱함수";
    case REMAINDER:  return "나머지함수";
    case FOLDING:    return "접지함수";
    }
    return "";
}

const char* resolve_name(ResolveType r) {
    switch (r) {
    case LINEAR_PROBING: return "선형조사법";
    case DOUBLE_HASHING: return "2차해쉬법";
    }
    return "";
}

int main(void) {
    int table[TABLE_SIZE];
    int insert_keys[100];
    int search_keys[40];

    // 난수 초기화 (재현성을 위해 고정 시드 사용)
    srand(42);

    // 0~1000 사이의 100개 임의의 숫자 생성 (삽입용)
    for (int i = 0; i < 100; ++i) {
        insert_keys[i] = rand() % 1001; // 0~1000
    }

    // 0~1000 사이의 40개 임의의 숫자 생성 (검색용)
    for (int i = 0; i < 40; ++i) {
        search_keys[i] = rand() % 1001;
    }

    for (int h = MID_SQUARE; h <= FOLDING; ++h) {
        for (int r = LINEAR_PROBING; r <= DOUBLE_HASHING; ++r) {
            Stats st = (Stats){ 0, 0, 0 };

            init_table(table);

            // 100개 키 삽입
            for (int i = 0; i < 100; ++i) {
                insert_key(table, insert_keys[i], (HashType)h, (ResolveType)r, &st);
            }

            // 40개 키 검색
            int found_count = 0;
            for (int i = 0; i < 40; ++i) {
                if (search_key(table, search_keys[i], (HashType)h, (ResolveType)r, &st)) {
                    found_count++;
                }
            }

            printf("\n=== 해쉬 함수: %s, 오버플로우 해결: %s ===\n",
                hash_name((HashType)h), resolve_name((ResolveType)r));
            printf("삽입한 키 개수          : %d\n", 100);
            printf("검색한 키 개수          : %d\n", 40);
            printf("검색 성공 개수          : %d\n", found_count);
            printf("충돌 횟수(해쉬 함수)     : %d\n", st.collisions);
            printf("오버플로우 비교 횟수(삽입): %d\n", st.overflow_comparisons);
            printf("검색 비교 횟수(전체)     : %d\n", st.search_comparisons);
        }
    }

    return 0;
}
