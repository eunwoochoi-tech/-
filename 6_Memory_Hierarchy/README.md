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



## Tests

### cache.c & cache-test.sh
 -> 접근하는 Memory의 영역의 크기에 따라서 속도가 얼마나 걸리는지 측정
 -> 결과를 보면 대략 cache사이즈를 경계로 소요시간이 계단식으로 변화한다(현재 내 컴퓨터의 cache는 32K, 32K, 512K, 16384K 이다)

### page-cache.sh & read-twice.sh
 -> Page-cache를 사용하기 이전과 사용한 후의 성능차이 비교
