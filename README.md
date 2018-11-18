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
### (Compare with Binary-Trie in previous project) ###

![](https://i.imgur.com/erzR1Ie.png) <br/>
![](https://i.imgur.com/05tHgse.png) <br/>
x: filter cases<br/>
y: memory used(kb)<br/>
<br/>
<br/>
![](https://i.imgur.com/SDS2pz1.png) <br/>
x: search levels<br/>
y: average finished time(sec)<br/>
<br/>
<br/>

## Conclusion ##
Both the Binary Trie and Byte Trie are match the complexity evaluation we done before, but Byte Trie only a litter faster than Binary Trie and takes too much space, so the Binary Trie is consider to be the most efficient for the original purpose.
<br/>
<br/>
<br/>
