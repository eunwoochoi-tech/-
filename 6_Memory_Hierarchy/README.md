# 실습과 그림으로 배우는 리눅스 구조 review

## Key Concepts

### Cache Memory(Main Memory -> Cache Memory)
 - 컴퓨터의 동작 흐름은 다음과 같다
  1) Memory에서 명령어를 읽어서 명령어를 바탕으로 메모리 -> 레지스터 로 데이터를 읽어들인다
  2) 레지스터에 있는 데이트러르 바탕으로 계산
  3) 계산 결과를 메모리에 rewrite
 - 여기서 Memory에 접근하는 1,3번은 2번 레지스터 접근에 비해 매우 느리다(수배 ~ 수백배)
 - 이렇게 되면 병목현상이 일어나기 때문에 Cache Memory를 사용하여 다음과 같은 순서로 latency를 감소시킨다(데이터 -> Cache memory -> Register)
 - Memory에서 Cache Memory로 데이터를 읽어올 때는 'Cache Line Size'로 읽어오며 요즘에는 보통 64B이다

### Cache Memory rewrite
 - CPU에서 연산을 진행하여 데이터가 변경되었다면 이를 다시 메모리에 쓰기위해 우선 캐시메모리에 변경된 데이터를 쓴 뒤 더티 flag를 활성화한다
 - 이후 백그라운드 처리로 더티 flag의 cache데이터를 memory에 rewrite 한다
 - Cache Memory가 꽉찼을 경우 새로운 공간의 memory에 접근할 때 기존의 특정 캐시 데이터를 지우고 새롭게 접근하는 data를 가져온다(더티 플래그가 활성화 되어있다면 rewrite 후에 처리)

### TLB(Translation Lookaside Buffer)
 - Process에서 가상주소에 접근할 때 다음과 같은 과정을 거친다
  1) Physical Memory에 있는 페이지 테이블을 참고하여 물리주소로 변환
  2) 변환된 물리주소에 접근하여 데이터를 가져옴
 - Cache Memory는 위 과정에서 2번만을 고속화한다. 1번을 고속화 하기 위해서 TLB가 생겼다
 - Cache Memory에 페이지 테이블과 비슷한 TLB를 놔둬서 2번을 고속화한다(나중에 더 알아보기)

### Page Cache(Disk -> Main Memory)
 - Cache Memory와 비슷한 개념으로 Page Cache는 저장장치 파일 내의 데이터를 메모리에 캐싱한 것이다
 - Page Cache는 Page단위로 캐싱을 진행
 - Process가 디스크에서 파일을 읽어들이면 바로 프로세스의 메모리에 데이터를 복사하는 것이 아닌 Kernel의 메모리 내에 있는 페이지 캐시라는 영역에 복사해놓은 뒤 프로세스의 메모리에 복사한다
 - 저장장치에 접근하는 경우보다 훨씬 더 빠르며 페이지 캐시는 전체 프로세스의 공유자원이므로 다른 프로세스가 접근할 수 있다.
 - 데이터를 쓸 때는 kernel의 페이지 캐시에 데이터를 쓴 후에 더티 플래그를 활성화한다(이 플래그가 붙은 페이지를 더티 페이지 라고 하며 백그라운드 처리로 저장장치에 반영된다)

### Buffer Cache
 - File System을 사용하지 않고 Device File을 이용하여 저장장치에 직접 접근하는 등의 목적으로 사용

### Mmap allocate page unit, malloc allocate byte unit
 - glibc의 malloc의 경우 사전에 mmap system call을 사용하여 커다란 memory pool을 생성하고 여기서 필요한 바이트만큼 반환해주는 형식

### Copy on Write
 - fork() System call을 수행할 때 부모 process의 memory를 자식 process에 전부 복사하는 것이 아닌 페이지 테이블만 복사한다.(이때 페이지 테이블 엔트리의 쓰기 권한은 부모,자식 모두 무효화됨)
 - 이후 페이지를 읽는것은 상관 없지만 메모리의 특정부분을 변경하고자 하면(즉, 페이지의 내용을 변경) 다음과 같은 흐름으로 진행된다.
   1) Page의 쓰기권한이 없기때문에 Page Fault가 발생(Page Fault Handler 실행)
   2) 접근한 페이지를 다른 장소에 복사하고 페이지에 접근한 프로세스에 할당 후 내용을 다시 작성
   3) 부모와 자식 프로세스 각각 공유가 해제된 페이지에 대응하는 페이지 테이블 엔트리를 업데이트
   4) 해당 Page는 더이상 공유하지 않으므로 양쪽(부모,자식) 모두 쓰기권한이 허가된다
 * fork()를 2번 호출 후 2번째 자식프로세스에서 특정 페이지에 쓰기를 진행하면 2번째 자식 프로세스에게만 페이지 테이블이 따로 할당될까? 아니면 자식1과 부모 프로세스도 따로 할당될까?
 * 주의할 점 : Copy on Write와 같은 상황에서는 전체 프로세스가 사용하는 process의 물리 메모리양이 실제 사용하고 있는 물리 메모리양보다 많다.

### Swaping
 - 물리 메모리의 양이 부족해지면 일부분을 저장장치에 저장하여 물리 메모리에 빈 공간을 생성한다. 이를 스왑핑이라고 한다
 - Swaping의 단위는 Page이며 이를 페이징(Paging)이라고도 한다.
 - 만약 물리 메모리 부족현상이 만성적으로 나타난다면 저장장치에 접근할 때 마다 swap-in swap-out이 발생하여 스래싱(thrashing)상태가 된다.(시스템의 속도가 현저히 낮아짐)
 - Swaping과 같이 저장장치에 대한 접근이 발생하는 Page Falut를 Major Fault라고 하며 이외의 것들을 Minor Fault라고 한다.(어느쪽이든 성능에 영향을 미치지만 Major Fault가 훨씬 느리다)

### Hierarchical Page Table
 - x64의 가상 주소의 크기는 128TB이기 때문에 이 모든 내용을 Page Table Entry로 가지고 있다면 총 256GB가 Page Table로 사용될 것이다. 이는 현실적으로 문제가 되기 때문에 x64 architecture의 경우 계층적인 페이지 테이블을 가지고 있다.(복잡하기 때문에 따로 정리)

### Huge Page
 - 한 페이지의 크기를 키워서 페이지 테이블을 작성하는 것.
 - 가상 메모리를 많이 사용하는 process에 대해서 Page Table에 필요한 메모리의 양이 줄어든다.

## Tests

### cache.c & cache-test.sh
 -> 접근하는 Memory의 영역의 크기에 따라서 속도가 얼마나 걸리는지 측정
 -> 결과를 보면 대략 cache사이즈를 경계로 소요시간이 계단식으로 변화한다(현재 내 컴퓨터의 cache는 32K, 32K, 512K, 16384K 이다)

### filemap.c
 -> file을 memory영역에 mapping시켜서 write 또는 fprintf와 같은 함수를 사용하지 않아도 memory영역에 데이터를 쓰면 파일의 내용도 바뀜(sync문제가 있는것으로 보임. 추후 다시 보기)

### demand-paging.c & vsz-rss.sh
 -> demand paging을 확인하기 위해 Memory를 할당받고 해당 영역에 접근하기 전/후의 Physical Memory 사용량을 비교
