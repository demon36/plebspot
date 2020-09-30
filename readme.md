#### what is this ?
quick and dirty blogging SofTwArE written on a hurry

#### rationale ?
write and manage content in markdown with minimal effort/footprint/cpu-cycles/interaction wizza cumba-yotar

#### how to build ?
- do a recursive clone
```
git clone --recursive --shallow-submodules git@github.com:demon36/plebspot.git
```
- cd and make the program
```
make
```
- currently mingw is required on windows, but the source code is portable

#### how to use ?
- initialize plebspot in an empty directory 
```sh
plebspot init
```
- add blog info to `pleb.yml`
- place markdown files in `pages` and `posts` directories
- place static content in `/static`
- run
```sh
plebspot serve
```

#### roadmap
- [x] use config file for icon, title, port, etc
- [x] fenced code blocks syntax highlighting
- [ ] optional custom post/page metadata (title, author, date, tags)
- [x] basic html templates
- [x] init command 
- [x] post categories/pages hierarchy
- [ ] logging
- [ ] error reporting
- [ ] optimization
- [ ] server side syntax highlighting (use enscript or src-highlite)
- [ ] compressed html cache
- [ ] github style task list
- [ ] rss feeds
- [ ] comments
- [ ] search
- [ ] meta tags
