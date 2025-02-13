#bin/bash

# g++ main.cpp -o build/main -std=c++23 -Wall -Wextra -Wformat-nonliteral -Wcast-align -Wpointer-arith -Wmissing-declarations -Winline -Wundef -Wcast-qual -Wshadow -Wwrite-strings -Wno-unused-parameter -Wfloat-equal -pedantic -fsanitize=address -fsanitize=leak

# ./build/main

# g++ deflate.cpp -o build/deflate -std=c++23 -Wall -Wextra -Wformat-nonliteral -Wcast-align -Wpointer-arith -Wmissing-declarations -Winline -Wundef -Wcast-qual -Wshadow -Wwrite-strings -Wno-unused-parameter -Wfloat-equal -pedantic -fsanitize=address -fsanitize=leak

# ./build/deflate

# g++ BitReader.cpp -o build/BitReader -std=c++23 -Wall -Wextra -Wformat-nonliteral -Wcast-align -Wpointer-arith -Wmissing-declarations -Winline -Wundef -Wcast-qual -Wshadow -Wwrite-strings -Wno-unused-parameter -Wfloat-equal -pedantic -fsanitize=address -fsanitize=leak

# ./build/BitReader

g++ xmlparser1.cpp xmlParser/xmlparser.cpp -o build/parser -g -std=c++23 -Wall -Wextra -Wformat-nonliteral -Wcast-align -Wpointer-arith -Wmissing-declarations -Winline -Wundef -Wcast-qual -Wshadow -Wwrite-strings -Wno-unused-parameter -Wfloat-equal -pedantic -fsanitize=address -fsanitize=leak -fsanitize=undefined 

./build/parser


# g++ test.cpp -o build/test -g -std=c++23 -Wall -Wextra -Wformat-nonliteral -Wcast-align -Wpointer-arith -Wmissing-declarations -Winline -Wundef -Wcast-qual -Wshadow -Wwrite-strings -Wno-unused-parameter -Wfloat-equal -pedantic -fsanitize=address -fsanitize=leak -fsanitize=undefined 

# ./build/test

# g++ test2.cpp -o build/test -g -std=c++23 -Wall -Wextra -Wformat-nonliteral -Wcast-align -Wpointer-arith -Wmissing-declarations -Winline -Wundef -Wcast-qual -Wshadow -Wwrite-strings -Wno-unused-parameter -Wfloat-equal -pedantic -fsanitize=address -fsanitize=leak -fsanitize=undefined 

# ./build/test