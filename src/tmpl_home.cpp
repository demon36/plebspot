const char* tmpl_home = R"(
<!DOCTYPE html>
<html>
<head>
	<title>{{blog_title}}</title>
</head>
<body>
	<h2>{{blog_title}}</h2>
	{{pages_list}}
	{{posts_list}}
</body>
</html>
)";