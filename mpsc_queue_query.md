#MPSC Queue  
  
**Single-consumer multi-producer concurrent queue**
  
##**Requirements:**

1.  FIFO(ish) queue  
2.  Flexible runtime size  
3.  Accept data from multiple producers (ideally via moving vs. copying) performantly.  It doesn't matter if items that are added around the same time are in a strict FIFO but it should trend towards fifo over longer periods.  
4.  Single consumer only.  Ideally should block until data available.  Should be able to retrieve data via move operation rather than copying. Performance for consumption not critical.  
5.  Modern C++ (move semantics)  
6.  Ideally header only  

##**Nice to have**  
  
1.  Header-only
2.  Lock free or at least relatively fast for producers.  Enqueue shouldn't take a whole lot longer than it takes to execute the move operation for the payload.  
3.  Consumer performance not nearly as critical.  
