for n in 10000
	do 
	./gen $n
    ./knnseq 10
		for nw in 1 2 3 4 5 6 7 8 16 32
		do
			for runs in 1 2 3
				do 
				./knnff $nw 10 0 1
				./knnff $nw 10 1 1
				./knn $nw 10 1
				done
		done							
	done

