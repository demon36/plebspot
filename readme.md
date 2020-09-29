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
- psst: build config is not so windows friendly but you can work your way through using mingw + gcc/clang

#### how to use ?
- initialize plebspot in an empty directory 
```sh
plebspot init
```
- edit `pleb.yml`
- place markdown files in `pages` and `posts` directories
- run 
```sh
plebspot
```

#### roadmap
- [x] use config file for icon, title, port, etc
- [x] fenced code blocks syntax highlighting
- [ ] optional custom post/page title instead of filename
- [x] basic html templates
- [x] init command 
- [x] post categories/pages hierarchy
- [ ] server side syntax highlighting (use enscript or src-highlite)
- [ ] compressed html cache
- [ ] rss feeds
- [ ] comments
- [ ] search
- [ ] meta tags
