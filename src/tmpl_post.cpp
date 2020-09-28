const char* tmpl_post = R"(
<!DOCTYPE html>
<html>
<head>
	<title>{{blog_title}} - {{post_title}}</title>
	<link rel="stylesheet" href="//cdnjs.cloudflare.com/ajax/libs/highlight.js/10.1.2/styles/monokai.min.css">
	<script src="//cdnjs.cloudflare.com/ajax/libs/highlight.js/10.1.2/highlight.min.js"></script>
	<script>hljs.initHighlightingOnLoad();</script>
</head>
<body>
	{{content}}
</body>
</html>
)";