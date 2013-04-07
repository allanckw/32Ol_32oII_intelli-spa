REM Usage: Call [AutoTester.exe] [Source] [Queries] [ out__.xml] > NUL
REM Just add to this file, this will run all test cases generated so far in sequence

Call $P@\SPA\Release\autotester.exe source1.txt  Queries1.txt  out1.xml > NUL
echo "Source 1 complete"

Call   $P@\SPA\Release\autotester.exe source2.txt  Queries2.txt  out2.xml > NUL
echo "Source 2 complete"

Call   $P@\SPA\Release\autotester.exe source3.txt  Queries3Follows.txt  out3Follows.xml > NUL
Call   $P@\SPA\Release\autotester.exe source3.txt  Queries3FollowsStar.txt  out3FollowsStar.xml > NUL
Call   $P@\SPA\Release\autotester.exe source3.txt  Queries3Modifies.txt  out3Modifies.xml > NUL
Call   $P@\SPA\Release\autotester.exe source3.txt  Queries3Parent.txt  out3Parent.xml > NUL
Call   $P@\SPA\Release\autotester.exe source3.txt  Queries3ParentStar.txt  out3ParentStar.xml > NUL
Call   $P@\SPA\Release\autotester.exe source3.txt  Queries3Uses.txt  out3ParentUses.xml > NUL
echo "Source 3 complete"

Call  $P@\SPA\Release\autotester.exe source4.txt  Queries4.txt  out4.xml > NUL
echo "Source 4 complete"

Call  $P@\SPA\Release\autotester.exe source5.txt  Queries5.txt  out5.xml > NUL
echo "Source 5 complete"

Call  $P@\SPA\Release\autotester.exe source6.txt  Queries6.txt  out6.xml > NUL
echo "Source 6 complete"

Call  $P@\SPA\Release\autotester.exe source7.txt  Queries7Next.txt  out7Next.xml > NUL
echo "Source 7 complete"

Call  $P@\SPA\Release\autotester.exe source8.txt  Queries8.txt  out8.xml > NUL
echo "Source 8 complete"

Call  $P@\SPA\Release\autotester.exe source9.txt  Queries9.1.txt  out9.1.xml > NUL

Call   $P@\SPA\Release\autotester.exe source9.txt  Queries9.2.txt  out9.2.xml > NUL
echo "Source 9 complete"

Call  $P@\SPA\Release\autotester.exe source10.txt  Queries10.txt  out10.xml > NUL
echo "Source 10 complete"