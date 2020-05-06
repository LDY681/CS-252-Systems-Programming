#!/usr/bin/python3
import subprocess;
import sys;

simpleTests = [('test_simple0', 2), \
               ('test_simple1', 3), \
               ('test_simple2', 3), \
               ('test_simple3', 3), \
               ('test_simple4', 3), \
               ('test_simple5', 3), \
               ('test_simple6', 3), \
               ];

mallocTests = [('test_exact', 5), \
               ('test_split', 5), \
               ('test_multi_malloc', 5), \
               ('test_insert_chunk', 3), \
               ('test_coalesce_chunk_insert', 3), \
               ('test_coalesce_chunk_coalesce', 4), \
               ('test_malloc_large', 5),
               ];

freeTests = [('test_free_insert', 3), \
             ('test_free_left', 4), \
             ('test_free_right', 4), \
             ('test_free_both', 3), \
             ('test_free_even', 5), \
             ('test_free_odd', 5), \
             ];

robustnessTests = [('test_large', 10), \
                   ('test_all_lists', 5) \
                   ];

otherTests = [('test_locks', 3), \
              ('test_malloc_zero', 2), \
              ('test_free_null', 2), \
              ('test_double_free', 2), \
              ('test_out_of_ram', 2), \
              ];

myTests = [('test_exact', 1),\
            # Add your additional tests here
            # They will be run when running `./runtest all`
            # and will not be counted in the total score
           ]

def color(c, s):
    return c + s + '\033[0m';

def red(s):
    return color('\033[31m', s);

def green(s):
    return color('\033[32m', s);

def yellow(s):
    return color('\033[33m', s);

def blue(s):
    return color('\033[34m', s);

def indent(indentby):
    for i in range(indentby):
        print('\t', end='');

def runTest(name, value, verbose, indentby):
    res = subprocess.check_output('./utils/difftest.sh ' + name, shell=True) \
                    .decode('utf-8');
    if (res != ''):
        indent(indentby);
        print(red('TEST: ' + name + ' FAILED ' + str(value)));
        if (verbose):
            print(res);
        return False;
    else:
        indent(indentby);
        print(green('TEST: ' + name + ' PASSED ' + str(value)));
        return True;

def runSuite(name, suite, verbose, indentby):
    totalPoints = 0;
    pointsEarned = 0;
    for test in suite:
        totalPoints += test[1];
    indent(indentby)
    print(blue('SUITE: ' + name + ' ' + str(totalPoints) + ' pts'));
    for test in suite:
        if (runTest(test[0], test[1], verbose, indentby + 1)):
            pointsEarned+=test[1];

    indent(indentby);
    if (pointsEarned == totalPoints):
        fmtColor = green;
    else:
        fmtColor = red;
    print(fmtColor('Subtotal: ' + str(pointsEarned) + '/' + str(totalPoints)));
    print();

    return (pointsEarned, totalPoints);

verbose = False;
pointsEarned = 0;
totalPoints = 0;

if (len(sys.argv) == 3 and sys.argv[1] == '-t'):
    runTest(sys.argv[2], 0, True, 0);
    exit(0);

ret = runSuite('Simple Tests', simpleTests, verbose, 0);
pointsEarned += ret[0];
totalPoints += ret[1];

ret = runSuite('Malloc Tests', mallocTests, verbose, 0);
pointsEarned += ret[0];
totalPoints += ret[1];

ret = runSuite('Free Tests', freeTests, verbose, 0);
pointsEarned += ret[0];
totalPoints += ret[1];

ret = runSuite('Other Tests', otherTests, verbose, 0);
pointsEarned += ret[0];
totalPoints += ret[1];

ret = runSuite('Robustness Tests', robustnessTests, verbose, 0);
pointsEarned += ret[0];
totalPoints += ret[1];

if (pointsEarned == totalPoints):
    fmtColor = green;
else:
    fmtColor = red;
print(fmtColor('\nTotal: ' + str(pointsEarned) + '/' + str(totalPoints)));

print('\nNOTE:\n' +
      'Additional tests worth an additional 10 points will be used for the final grading');

if (len(sys.argv) == 2 and sys.argv[1] == 'all'):
    print();
    ret = runSuite('My Tests', myTests, verbose, 0);
