FLAGS = --smart --normalize --standalone --mathml -t html5
default:docs

docs:
	find . -name \*.textile -type f -exec pandoc $(FLAGS) -o html/{}.html {} \;
	rename 's/\.textile\.html/\.html/' html/*.textile.html
