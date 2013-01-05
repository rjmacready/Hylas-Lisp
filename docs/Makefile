DOCBASEFLAGS = -f markdown --smart --normalize --mathml -t html5\
	--css=../res/css/style.css --css=../res/css/hylas.css\
	--highlight-style pygments
TOCFLAGS = $(DOCBASEFLAGS) --template=res/templates/index.template
DOCFLAGS = $(DOCBASEFLAGS) --template=res/templates/doc.template
default:docs

docs:
	find . -name \*.md -type f -exec pandoc $(DOCFLAGS) -o html/{}.html {} \;
	rename 's/\.md\.html/\.html/' html/*.md.html
	pandoc $(TOCFLAGS) -o html/index.html index.md

doxygen:
	doxygen
clean:
	rm html/*