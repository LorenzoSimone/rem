for n in 1000 1500 2000 2500 3000 3500 4000
	do ./gen $n;
	for r in 1 2 3 4 5 6 7 8 9 10
		do
		for sched in "s" "ns"
			do ./knnff 8 10 $sched 0 0 0;
			done
		done
	done
		