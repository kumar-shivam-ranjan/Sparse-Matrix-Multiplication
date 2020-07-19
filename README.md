# Sparse-Matrix-Multiplication
Code for heterogeneous computing of product of two sparse matrices<br>


### Algorithm: Gustavson’s Row-wise SpGEMM 3<br>
Input: Sparse matrices A and B<br>
Output: Sparse matrix C<br>
<pre> 
set matrix C to ∅<br>
for all a i ∗ in matrix A in parallel do<br>
  for all a ik in row a i ∗ do<br>
    for all b k j in row b k ∗ do<br>
			value ← a ik b k j<br>
			if c i j #∈ c i ∗ then<br>
				insert ( c i j ← v alue ) to c i ∗<br>
			else<br>
				c i j ← c i j + value<br>
			end if<br>
		end for<br>
	end for<br>
end for<br>
</pre>

## Algorithm 2 RowsToThreads.
Input: Sparse matrices A and B<br>
Output: Array o f f set
<pre>
{1. Set FLOP vector}
for i ← 0 to m in parallel do
	flop [ i ] ← 0
	for j ← r pts A [ i ] to r pts A [ i + 1] do
		rnz ← r pts B [ cols A [ j] + 1] − r pt B [ cols A [ j]]
		flop [ i ] ← flop [ i ] + rnz
	end for
end for
{ 2 . Assign rows to thread }
flop ps ← ParallelPrefixSum ( flop )
sum flop ← flop ps [ m ]
tnum ← omp_get_max_threads ()
a v e flop ← sum flop / tnum
o f f set[0] ← 0
for tid ← 1 to tnum in parallel do
	o f f set [ tid ] ← lowbnd ( flop ps , a v e flop ∗ tid )
end for
o f f set [ t num ] ← m
</pre>
## Algorithm : Hash SpGEMM.
Input: Sparse matrices A and B<br> 
Output: Sparse matrix C
<pre>
off set ← RowsToThreads ( A, B )
{Determine hash-table size for each thread}
tnum ← omp_get_max_threads ()
for tid ← 0 to tnum in parallel do
	size t ← 0
	for i ← o f f set [ t id] to o f f set [ t id + 1] do
		size t ← max ( size t , flop [ i ] )
	end for
	{Required maximum hash-table size is N col }
	size t ← min ( N col , size t )
	{Return minimum 2 n so that 2 n > size t }
	size t ← lowest_p2 ( size t )
end for
Symbolic ( r pts C , A, B )
Numeric ( C, A, B )
<pre>
