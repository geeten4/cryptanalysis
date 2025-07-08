# HW project for Cryptanalysis course

To compile using gcc and run the labworks, use a shortcut

<pre lang="markdown">
$ ./cryptanalysis.sh X
</pre>

where X is the number of the corresponding labwork, or the keyword "final" for the final HW.
(If shell script cannot be run due to permissions, change them via)


<pre lang="markdown">
$ chmod u+x cryptanalysis.sh
</pre>


## Third homework

generate random permutation with `/labwork3/python/gen_random_permutation.py` via

```bash

python3 ./labwork3/python/gen_random_permutation.py

```

and it stores a random permutation of GF(2^12) in `/labwork3/data/random_permutation.txt` along
with its inverse in `/labwork3/data/random_inverse_permutation.txt`

#### NOTE
regenerating the permutation means the message-ciphertext pairs are incompatible with the permutation,
we have to regenerate message-ciphertext pairs as well

To generate random message-ciphertext pairs, first compile the encryption functions to a shared .so object

```bash

make encryption

```

this compiles to a shared object `/bin/encryption.so`

# TODO: accept t as parametr for python script

and run the python script for random pairs generation

```bash

python3 ./labwork3/python/gen_message_ciphertext_pairs.py

```

it stores the pairs in `/labwork3/data/message_ciphertext.csv`



# Final homework

run tests: compile to shared object using

```bash

make dl

```

which creates a dl.so shared object file in bin. This is used to run python tests. Run them via

```bash

python3 labwork_final/python/tests.py

```
