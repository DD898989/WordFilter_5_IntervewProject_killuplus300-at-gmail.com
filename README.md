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
N: Input Length<br/>
M: Filter Cases<br/>
<br/>


## Implementation ##

## Analysis ##

![](https://i.imgur.com/pW9tTEc.png) <br/>
x: filter cases<br/>
y: memory used(kb)<br/>
dialog length(char):500<br/>
<br/>
The result not seem to be a clear direct proportion because there is a special array-resizing funcion were used<br/>
<br/>
![](https://i.imgur.com/7guqIaN.png) <br/>
x: filter cases<br/>
y: average finished time(sec)<br/>
dialog length(char):500<br/>
<br/>
As expect, filter cases are not relative to time<br/>
<br/>
![](https://i.imgur.com/gNDcBKR.png) <br/>
x: dialog length(char)<br/>
y: average finished time(sec)<br/>
filter cases:216000<br/>
<br/>
As expect, dialog length are in direct proportion to time<br/>
<br/>
