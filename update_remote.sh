#!/bin/bash

#scp -o ProxyJump=sarahbh@login.cs.unc.edu benchmark_tests.c sarahbh@adams.cs.unc.edu:
#scp -o ProxyJump=sarahbh@login.cs.unc.edu Makefile sarahbh@adams.cs.unc.edu:
#/scp -o ProxyJump=sarahbh@login.cs.unc.edu unit_tests.py sarahbh@adams.cs.unc.edu:

scp -o ProxyJump=sarahbh@login.cs.unc.edu unit_tests.py ttest.py Makefile benchmark_tests.c sarahbh@adams.cs.unc.edu:
