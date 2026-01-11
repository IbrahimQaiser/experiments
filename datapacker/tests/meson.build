gtest_dep = dependency('gtest')

datapacker_test = executable(
    'datapacker_test',
    sources: ['datapacker_test.cpp'],
    dependencies : [ gtest_dep ],
    include_directories: include_dirs,
    cpp_args: extra_args
)
test('datapacker_test', datapacker_test)

