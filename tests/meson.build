
tests = [
  'word_is_valid',
  'find_candidate',
]

foreach t : tests
  warning(t)

  src_test = 'test_' + t + '.c'
  warning(src_test)

  test_exec = executable(
    t,
    'test.c',
    src_test,
    src,
    include_directories: [
      inc,
      test_inc,
    ],
    c_args: flags + ['-DUNIT_TEST_TARGET'],
    dependencies: [ x11, x11_test ],
  )

  test(t, test_exec)
endforeach
