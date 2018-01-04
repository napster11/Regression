knn :
	g++ -o knn knn.cpp

regression :
	g++ -o regression regression.cpp

nnregression :
	g++ -o nnregression nn_regression.cpp


clean:
	rm -f *.o