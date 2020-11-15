const char* plain_html_tmpl = R"(
<!DOCTYPE html>
<html lang="en-US">
<head>
	<meta charset="UTF-8">
	<meta name="description" content="{{page_desc}}">
	<meta name="keywords" content="{{keywords}}">
	{{#author}}
	<meta name="author" content="{{author}}">
	{{/author}}
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<title>{{blog_title}} - {{page_desc}}</title>
	<link rel="alternate" type="application/rss+xml" title="{{blog_title}} - Feed" href="/feed">
	<link rel="icon" href="/static/logo.png">
	<link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css" integrity="sha384-JcKb8q3iqJ61gNV9KGb8thSsNjpSL0n8PARn9HuZOnIxN0hoP+VmmDGMN5t9UJ0Z" crossorigin="anonymous">
	<link rel="stylesheet" href="//cdnjs.cloudflare.com/ajax/libs/highlight.js/10.1.2/styles/default.min.css">
	<script src="//cdnjs.cloudflare.com/ajax/libs/highlight.js/10.1.2/highlight.min.js"></script>
	<script>hljs.initHighlightingOnLoad();</script>
</head>
<body>
	<div class="container">
		<div class="row" style="padding-top:2em;">
			<div class="sidebar col-sm-4">
				<h1>
					<a href="/" rel="home">
						<img src="/static/logo.png">
							{{blog_title}}
					</a>
				</h1>
				<p>{{blog_desc}}</p>

				<h4>Pages</h4>
				<ul>
					{{#pages_list}}
					<li>
						<a href="{{url}}">{{title}}</a>
					</li>
					{{/pages_list}}

					{{#cat_pages_list}}
					<li>
						{{cat_title}}
						<ul>
							{{#children}}
							<li>
								<a href='{{url}}'>{{title}}</a>
							</li>
							{{/children}}	
						</ul>
					</li>
					{{/cat_pages_list}}

				</ul>

				<h4>Posts</h4>
				<ul>
					{{#posts_list}}
					<li>
						<a href="{{url}}">{{title}}</a>
					</li>
					{{/posts_list}}
					
					{{#cat_posts_list}}
					<li>
						{{cat_title}}
						<ul>
							{{#children}}
							<li>
								<a href='{{url}}'>{{title}}</a>
							</li>
							{{/children}}	
						</ul>
					</li>
					{{/cat_posts_list}}
				</ul>
			</div>

			<div class="content col-lg-8">
				<dl class="row">
					{{#date}}
						<dt class="col-sm-3">date</dt>
						<dd class="col-sm-9">{{date}}</dd>
					{{/date}}
					{{#author}}
						<dt class="col-sm-3">author</dt>
						<dd class="col-sm-9">{{author}}</dd>
					{{/author}}
					{{#category}}
						<dt class="col-sm-3">category</dt>
						<dd class="col-sm-9">{{category}}</dd>
					{{/category}}
				</dl>

				{{{content}}}
			</div>
		</div>
	</div>
</body>
</html>
)";