const char* tmpl_home = R"(
<!DOCTYPE html>
<html>
<head>
	<title>{{blog_title}}</title>
</head>
<body>
	<h2>{{blog_title}}</h2>
	<h4>pages</h4>
	<ul>
		{{#pages_list}}
		<li><a href='/pages/{{url}}'>{{title}}</a></li>
		{{/pages_list}}

		{{#cat_pages_list}}
		<li>{{cat_title}}
			<ul>
				{{#children}}
				<li><a href='/pages/{{url}}'>{{title}}</a></li>
				{{/children}}	
			</ul>
		</li>
		{{/cat_pages_list}}
	</ul>

	<h4>posts</h4>
	<ul>
		{{#posts_list}}
		<li><a href='/posts/{{url}}'>{{title}}</a></li>
		{{/posts_list}}
		
		{{#cat_posts_list}}
		<li>{{cat_title}}
			<ul>
				{{#children}}
				<li><a href='/posts/{{url}}'>{{title}}</a></li>
				{{/children}}	
			</ul>
		</li>
		{{/cat_posts_list}}
	</ul>

</body>
</html>
)";