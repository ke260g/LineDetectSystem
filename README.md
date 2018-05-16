# LineDetectSystem [deprecated]
## Graduation Design from 2018-04 to 2018-05
### Brief
+ depends: opencv2.4 and qt5
+ run-cmd: `$_  qmake && make && ./LineDetectSystem`
+ functionality: `capture > canny > houghP > dbscan`

### Features
+ convert between JSON and {QVector or QMap}
+ convert between QTreeWidget and {QVector or QMap}
+ a classical QThread usage demo.
+ dbscan implementation and a usage demo

### Not completed Or Bugs
+ button **Export Result** is not implemented.
+ Debug message is not allow to disbale in dbscan.
+ Debug message in dbscan is not enough for developement.
+ A ugly or freak distance function in dbscan.
+ MinD and MinN is not allow to control dbscan.

### Screenshots
![image](screenshots/mwin.png?raw=true "")
