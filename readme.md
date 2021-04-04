#### what is this ?
markdown blogging utility written on a hurry

#### rationale ?
write and manage content in markdown with minimal effort/footprint/cpu-cycles/interaction with computer

#### how to build ?
- do a recursive clone and change dir
```
git clone --recursive --shallow-submodules git@github.com:demon36/plebspot.git
cd plebspot
```
- via cmake:
```
cmake . -Bbuild && cmake --build build
```
- via make on gnu/linux (must have `libssl-dev` or alternative pre-installed):
```
make
```

#### how to use ?
- initialize plebspot in an empty directory 
```sh
plebspot init
```
- adjust configuration in `pleb.yml`
- place markdown files in `pages` and `posts` directories with metadata syntax like:
```
[title: a friendly post tile instead of the plain file name]::
[author: me]::
[date: 19 Dec 2019]::
[tags: fun, unfun, ofc these md comments look nasty]::
```
- place static content in `static` folder (ex: `logo.png`, `favicon.ico`)
- adjust `template/plain.html` or add new template
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
- [x] rss feeds
- [x] sitemap xml
- [x] ssl

#### luxury features
- [ ] `--daemonize`
- [ ] status, restart, stop commands
- [ ] auto config reload
- [ ] rendered html cache
- [ ] visitor statistics
- [ ] server side syntax highlighting (use enscript or src-highlite)
- [ ] github style task list
