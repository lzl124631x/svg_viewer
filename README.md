svg_viewer
==========

A Tiny SVG Render Engine written in C/C++.


---

Wow! It's so fascinating reviewing this repo.

I spent half a year during my postgraduate implementing HTML render engine. It's way too difficult for one person, I was exhausted and found that it's really hard to expand the code due to my inexperience.

My professor recommended me to build and learn from WebKit. I did it and learned a lot. And at the same time, he recommended me to implement a relatively much easier render engine -- SVG render engine, because there are much less types of elements in SVG than HTML. So I switched to implement this SVG render engine.

Sadly, at that time I don't have any idea about version control, so my HTML render engine ended up not working and I don't have time to fix it.

The good news is that, due to my former experience in implementing HTML render engine, it became much easier to implement one for SVG. The key parts are only six basic shapes (Rectangle, Circle, Ellipse, Line, Polygon and Polyline) and `<path>` element.

Within the first week, I came up with a prototype that could render an SVG! Woohoo!

Later I spent a lot of time fixing bugs, implementing algorithms e.g. matrix transformation, elliptical arc, alpha-bending, antialiasing.

The following screenshots are worth a thousand words.

![[font]musixdoc.png](https://raw.githubusercontent.com/lzl124631x/svg_viewer/master/screenshots/[font]musixdoc.png)
![3d4.png](https://raw.githubusercontent.com/lzl124631x/svg_viewer/master/screenshots/3d4.png)
![b8.png](https://raw.githubusercontent.com/lzl124631x/svg_viewer/master/screenshots/b8.png)
![clinton.png](https://raw.githubusercontent.com/lzl124631x/svg_viewer/master/screenshots/clinton.png)
![cowboy.png](https://raw.githubusercontent.com/lzl124631x/svg_viewer/master/screenshots/cowboy.png)
![glidingball.png](https://raw.githubusercontent.com/lzl124631x/svg_viewer/master/screenshots/glidingball.png)
![helix.png](https://raw.githubusercontent.com/lzl124631x/svg_viewer/master/screenshots/helix.png)
![induction.png](https://raw.githubusercontent.com/lzl124631x/svg_viewer/master/screenshots/induction.png)
![tiger.PNG](https://raw.githubusercontent.com/lzl124631x/svg_viewer/master/screenshots/tiger.PNG)
![tplot.png](https://raw.githubusercontent.com/lzl124631x/svg_viewer/master/screenshots/tplot.png)
![xenia4.PNG](https://raw.githubusercontent.com/lzl124631x/svg_viewer/master/screenshots/xenia4.PNG)

# License
See the [LICENSE](./LICENSE.md) file for license rights and limitations (MIT).
