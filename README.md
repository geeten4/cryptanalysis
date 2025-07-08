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

Generate a random permutation with `./labwork3/python/gen_random_permutation.py` via

<pre lang="markdown">
python3 ./labwork3/python/gen_random_permutation.py
</pre>

The script stores a random permutation of GF(2^12) in `./labwork3/data/random_permutation.txt` along
with its inverse in `./labwork3/data/random_inverse_permutation.txt`

#### NOTE
Regenerating the permutation means the message-ciphertext pairs are incompatible with the permutation,
we have to regenerate message-ciphertext pairs as well

To generate random message-ciphertext pairs, first compile the encryption functions to a shared .so object via

<pre lang="markdown">
make encryption
</pre>

which compiles to a shared object `./bin/labwork3/encryption.so`

##### TODO: accept t as a parameter for python script (now is hard-coded in the script)

Now you can run the python script for random message-ciphertext pairs generation

<pre lang="markdown">
python3 ./labwork3/python/gen_message_ciphertext_pairs.py
</pre>

which stores the pairs in `./labwork3/data/message_ciphertext.csv`



# Final homework

Extensive tests are implemented for the final Discrete logarithm solver. First compile to a shared .so object via

<pre lang="markdown">
make dl
</pre>

which creates a dl.so shared object file in `./bin/labwork_final/`. This is used to run python tests. Run them via

<pre lang="markdown">
python3 ./labwork_final/python/tests.py
</pre>

which shows all the available tests to be run.
