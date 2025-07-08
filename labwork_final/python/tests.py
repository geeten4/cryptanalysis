import sys
from tests_functions import print_usage, batch_test, fb_limit_test


def bad_usage() -> None:
    print_usage()
    sys.exit(1)


if len(sys.argv) < 2:
    bad_usage()

arg1 = sys.argv[1]

match arg1:
    case "batch":
        try:
            arg2, arg3, arg4, arg5 = sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5]
        except Exception:
            bad_usage()

        if arg2.isnumeric() and arg3.isnumeric():
            test_count = int(arg2)
            lower = int(arg3)
        else:
            bad_usage()

        if arg4 == 'theoretical':
            fb_limit = 'theoretical'
        elif arg4.isnumeric():
            fb_limit = arg4
        else:
            bad_usage()

        if arg5 == 'true':
            verbose = True
        elif arg5 == 'false':
            verbose = False
        else:
            bad_usage()

        batch_test(test_count=test_count, lower=lower, fb_limit=fb_limit, verbose=verbose)

    case "fb_limit":
        try:
            arg2, arg3, arg4 = sys.argv[2], sys.argv[3], sys.argv[4]
        except Exception:
            bad_usage()

        if arg2.isnumeric() and arg3.isnumeric() and arg4.isnumeric():
            lower = int(arg2)
            upper = int(arg3)
            sample_count = int(arg4)
        else:
            bad_usage()


        fb_limit_test(lower=lower, upper=upper, sample_count=sample_count)

    case _:
        bad_usage()
