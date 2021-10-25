for n in 1000 3000 5000 8000 10000
	do ./gen $n;
	for sched in "s" "ns"
			do
			for s1 in 0 1
					do
					for s2 in 0 1
						do
						for s3 in 0 1
							do 
								for nw in 1 2 3 4 5 6 7 8
									do
									for runs in 1 2 3 4 5
										do ./knnff $nw 10 $sched $s1 $s2 $s3;
						done
					done							
				done
			done
		done
	done
done