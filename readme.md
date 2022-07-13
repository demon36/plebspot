### plebspot

##### a markdown blogging utility written on a hurry

#### rationale:

write and manage content in markdown with minimal effort/footprint/cpu-cycles/interaction with 
computer

#### features:
- comments + captcha
- categories for posts/pages
- minimal js, only used for fenced code blocks syntax highlighting
- optional custom post/page metadata (title, author, date, keywords)
- basic single file html templates
- rss feeds
- sitemap xml
- ssl

#### build instructions:

```sh
git clone --recursive --shallow-submodules git@github.com:demon36/plebspot.git
cd plebspot
cmake . -Bbuild
cmake --build build -j4
```

#### basic usage:

```sh
mkdir myblog
cd myblog
plebspot init
plebspot serve
echo "#### my new post" > posts/newpost.md
```

#### more usage details:

- configure serve port, ssl, blog info, template, comments & search keywords by adjusting `pleb.yml`
- directories inside `pages` and `posts` represent categories (ex: `posts/my_category/sample_post.md`)
- customize page/post metadata by using this syntax:
```
[title: a friendly post title instead of the plain file name]::
[author: me]::
[date: 19 Dec 2019]::
[tags: fun, unfun, ofc these md comments look nasty]::

post contents here
```
- comments are stored in plain text files (ex: `posts/my_category/sample_post.md.comments`), you can edit them as needed
- static content can be placed in `static` folder (ex: `logo.png`, `favicon.ico`) as needed
- customize single file template `template/plain.html` or add new one if needed

#### live example: [psilocyber.tech](https://psilocyber.tech/)

#### todo:
- [ ] `--daemonize`
- [ ] status, restart, stop commands
- [ ] use json for comments
- [ ] rendered html cache
- [ ] visitor statistics
- [ ] server side syntax highlighting (use enscript or src-highlite)
- [ ] github style task list
