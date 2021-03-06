project(
  'wordle',
  'c',
  default_options : [
    'c_std=gnu11',
  ]
)

c_compiler = meson.get_compiler('c')
root_dir = meson.source_root()

flags = c_compiler.get_supported_arguments(
  [
    '-Wall',
    '-Werror',
    '-D_GNU_SOURCE',
    '-Wunused',
    '-Wunused-value',
    '-Wunused-variable',
    '-Wunused-parameter',
    '-Wbool-compare',
    '-Wformat-security',
    '-Winit-self',
    '-Wlogical-op',
    '-Wtrampolines',
    '-Wmissing-format-attribute',
    '-Wno-error=deprecated-declarations',
    '-Wno-format-zero-length',
    '-Wshadow',
    '-Wmissing-declarations',
    '-Wpointer-arith',
    '-Wformat=2',
    '-pipe',
    '-pthread',
    '-g3',
  ]
)

inc = include_directories('src')
test_inc = include_directories('tests')

src = files(
  'src/color.c',
  'src/status.c',
  'src/words.c',
  'src/wordle.c',
  'src/coord.c',
  'src/dump.c',
  'src/utils_x11.c',
  'src/options.c',
  'src/nerdle/lexer.c',
  'src/nerdle/parser.c',
  'src/nerdle/equation.c',
  'src/nerdle/nerdle.c',
)

cc = meson.get_compiler('c')
x11 = dependency('x11', required: false, disabler: true)
x11_test = dependency('xtst', required: false, disabler: true)

subdir('tests')

applications = [
  'wordle_louan',
  'octordle',
  'nerdle',
  'bench_nerdle',
]

foreach app : applications
  executable(
    app,
    src,
    'app/' + app + '.c',
    include_directories: inc,
    c_args: flags,
    dependencies : [ x11, x11_test ],
  )
endforeach
