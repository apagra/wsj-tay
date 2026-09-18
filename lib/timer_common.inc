  integer :: level, onlevel(0:1000)
  common/timer_private/ level, onlevel
! threadprivate disabled: gfortran emits .tls_common for a threadprivate COMMON
! block, which the PE/COFF assembler cannot encode. The timer body runs inside
! !$omp critical(timer), so sharing is race-free; nesting levels from concurrent
! threads just interleave, hence the enlarged onlevel bound.
