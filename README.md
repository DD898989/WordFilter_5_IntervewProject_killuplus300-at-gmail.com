# WordFilter_5_IntervewProject_killuplus300-at-gmail.com

## Complexity Evaluation ##

#### Double Array Trie ####
![](http://www.csie.ntnu.edu.tw/~u91029/Trie5.png) <br/>
*from http://www.csie.ntnu.edu.tw/~u91029/String.html*
<br/>
<br/>
<br/>

|               |Time             |Space          |
| ------------- |:---------------:|:-------------:|
| DAT           | `O(N)`          |`O(M)`         |

<br/>
N: Input Length(Search Level)<br/>
M: Filter Cases<br/>
<br/>

## Analysis ## 
![](https://i.imgur.com/erzR1Ie.png) <br/>
### (Compare with Binary-Trie in previous project) ###
![](https://i.imgur.com/fEJyoFk.png) <br/>
x: search levels<br/>
y: average finished time(sec)<br/>
<br/>
<br/>
![](https://i.imgur.com/05tHgse.png) <br/>
x: filter cases<br/>
y: memory used(kb)<br/>
<br/>
<br/>

## Conclusion ##
Binary Trie and DAT have same time complexity of `O(search level)` and space complexity `O(filter cases)`, but DAT is much more efficient in memory usage
<br/>
<br/>
<br/>
