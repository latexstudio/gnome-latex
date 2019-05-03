History of LaTeXila/GNOME LaTeX
===============================

I (Sébastien Wilmet) started LaTeXila because I wanted to work on a programming
project during the summer (I was a student at that time). I didn't know which
subject to choose, and I've chosen LaTeX because there was a lack of a good
LaTeX editor based on GTK (i.e. well integrated to GNOME).

The development started in August 2009, in the C language. In the following
months a few versions were released (0.0.1, 0.0.2, 0.1 and 0.2). It had all the
major basic features, and worked quite well. On the other hand, the code
architecture was a disaster, due to a lack of experience and a lack of a good
and recent GLib/GTK introduction for the C language (something that I've now
fixed by writing
[this guide](https://people.gnome.org/~swilmet/glib-gtk-book/)).
Since I like statistics: at the time of the 0.2 version there was 200 Git
commits and 10k lines of C code. The project was hosted entirely on
SourceForge.

In June 2010 I started the rewrite of LaTeXila in Vala. Vala seemed easier,
with a good tutorial and the ability to write Object-Oriented code easily.
LaTeXila 2.0 was released in November 2010. It had all the features of the 0.2
version, plus many improvements. The Git repository was hosted on GitHub, but
the project homepage and downloads were still on SourceForge. The 0.2 -> 2.0
transition reflects the rewrite in Vala. Numbering that version “1.0” was not
suitable, because it would have been seen as an achievement of the 0.x series.
Also, with the 2.0 version number, the GNOME version numbering scheme has been
adopted (even/odd minor version for stable/unstable releases). So the 2.0
version was the achievement of the Vala rewrite, but at the same time a new
beginning for the future of LaTeXila.

In February-June 2011, I worked on LaTeXila as part of a project for my studies
(for my end of bachelor studies project, for which I've had a good grade,
20/20 :p). In the summer 2011, the project was moved to gnome.org, after
reading [this series of articles](http://blogs.gnome.org/johannes/2010/06/04/why-gnome-org/).
GitHub was not convenient for handling translations. During
that period I've also read the book Code Complete
([blog post](http://blogs.gnome.org/swilmet/2012/08/01/about-code-quality-and-maintainability/)),
which improved a lot my programming skills. That's why there has been a lot of
code cleanups during that summer and the next summer.

With time LaTeXila has seen more and more features. During the summer 2012,
another notable change is the switch from CMake to Autotools for the build
system ([blog post](http://blogs.gnome.org/swilmet/2012/09/05/switch-from-cmake-to-autotools/)).
In 2012 I also started to contribute to GtkSourceView, because there were some
bugs in the completion system. And with LaTeXila in good shape, I wanted to
work on something else. I've done a Google Summer of Code in 2013 for
GtkSourceView. Since I was more or less the only developer of LaTeXila, there
was less activity in LaTeXila. But the work in GtkSourceView is of course
beneficial for LaTeXila.

In September 2014, LaTeXila has made a jump from the 2.12 version to 3.14, to
finally have the same versions as GNOME. It is easier to know with which GNOME
version a certain LaTeXila version was developed against.

For the 3.16 version, in March 2015, the build tools engine has been completely
re-written in C, to have more robust and modern code, with a better code
architecture. By contributing to GtkSourceView, I've learned how to write
Object-Oriented code in C with the GObject library. And over time I was less
and less happy with Vala, I prefer C/GObject for developing a GTK application
or library. So I've started a rewrite of LaTeXila in C, but not a complete
rewrite like it was the case for the 2.0 version in Vala, since from the Vala
code it is possible to use features developed in C. So starting with the 3.16
version some chunks of code have been progressively rewritten in C/GObject,
fully documented with GTK-Doc comments and GObject Introspection annotations.

In March 2015, a first fundraiser has been launched. More than €1000 has been
raised, which permitted me to improve the spell-checking and to start the
development of the gspell library. (I was no longer a student at that time).

In September 2016, a second fundraiser has been launched in parallel with a
gspell fundraiser.

TODO: continue history with the creation of the Tepl and Amtk libraries, the
rename of LaTeXila to GNOME LaTeX, and the upcoming 10 years anniversary.

That's the end for now. I hope there will be other important events to relate.
And *you* can be part of this history!

Thanks to all contributors of GNOME LaTeX: developers, translators, packagers,
designers, donors and everybody else!
