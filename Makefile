sample: sample.cpp
	g++ $< -o $@ -O3

clean:
	rm -f sample
