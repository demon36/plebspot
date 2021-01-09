#### what is this ?
quick and dirty blogging SofTwArE written on a hurry

#### rationale ?
write and manage content in markdown with minimal effort/footprint/cpu-cycles/interaction wizza cumba-yotar

#### how to build ?
- do a recursive clone and change dir
```
git clone --recursive --shallow-submodules git@github.com:demon36/plebspot.git
cd plebspot
```
- for linux:
```
make
```
- for windows and mac meson should work, thanks to @Abdullah-AlAttar

#### how to use ?
- initialize plebspot in an empty directory 
```sh
plebspot init
```
- add blog info to `pleb.yml`
- place markdown files in `pages` and `posts` directories
- place static content in `static` folder
- run
```sh
plebspot serve
```

#### roadmap
- [x] use config file for icon, title, port, etc
- [x] fenced code blocks syntax highlighting
- [x] optional custom post/page metadata (title, author, date, keywords)
- [x] basic html templates
- [x] init command 
- [x] post categories/pages hierarchy
- [x] captcha
- [x] comments
- [ ] store comment author + date
- [ ] show old comment message at captcha failure
- [ ] site wide and post specific comments config
- [ ] prevent rendering .comments file
- [ ] rss feeds
- [ ] generate sitemap xml
- [ ] logging
- [ ] server side syntax highlighting (use enscript or src-highlite)
- [ ] compressed html cache
- [ ] github style task list
- [x] meta tags
