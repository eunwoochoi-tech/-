# 실습과 그림으로 배우는 리눅스 구조 review

## Key Concepts

### Virual Memory?
 - Process는 물리 메모리에 직접 접근하지 않고 가상 주소를 사용하여 간접적으로 접근한다(Kernel이 가상주소와 물리주소의 변환을 지원)

### Page Table?
 - 전체 메모리는 페이지라는 단위로 나뉘어서 관리된다. 이때 가상주소와 물리주소의 변환은 페이지 단위로 이루어진다.
 - 한 Page에 대한 데이터를 Page Table Entry라고 하며 이의 집합을 Page Table이라고 한다

### Page Fault?
 - 가상 주소가 물리 주소와 매핑되어있지 않는 경우 Page Fault Interrupt Handler가 발생한다. 또는 올바르지 않은 가상 주소로의 접근역시 동일하게 인터럽트 핸들러가 발생한다.

### Demand Paging?
 - Process에게는 Virtual address가 할당되었지만 실제 Physical address는 할당되지 않은 상태를 추가함
 - 실제 Process에서 해당 address에 접근할 때 Physical address가 할당됨

### How is allocated memory to a process
 1. Request memory from a process(ex: malloc, mmap etc...)
 2. Kernel allocate new memory space(update page table)
 3. Return a virtual address to a process

### Mmap allocate page unit, malloc allocate byte unit
 - glibc의 malloc의 경우 사전에 mmap system call을 사용하여 커다란 memory pool을 생성하고 여기서 필요한 바이트만큼 반환해주는 형식


## Tests

### mmap.c
 -> mmap을 통한 메모리 할당 전,후의 virtual address list를 비교

### filemap.c
 -> file을 memory영역에 mapping시켜서 write 또는 fprintf와 같은 함수를 사용하지 않아도 memory영역에 데이터를 쓰면 파일의 내용도 바뀜(sync문제가 있는것으로 보임. 추후 다시 보기)

### demand-paging.c & vsz-rss.sh
 -> demand paging을 확인하기 위해 Memory를 할당받고 해당 영역에 접근하기 전/후의 Physical Memory 사용량을 비교
