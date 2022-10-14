[![KapEngine](https://img.shields.io/badge/KapEngine-brightgreen.svg)](https://github.com/benji-35/KapEngine/)
[![KapMirror](https://img.shields.io/badge/KapMirror-brightgreen.svg)](https://github.com/Chaika9/KapMirror/)

<img src="https://static.miraheze.org/mgqwiki/b/bf/Sylph_Inside.png" title="Sylph"/>

~~ Like UDP, the wind carries information, but sometimes it gets lost.

Simple, message based, allocation free MMO Scale UDP networking in C#. And without wind magic.
Designed for an Epitech project (UDP required)

Sylph was designed with the [KISS Principle](https://en.wikipedia.org/wiki/KISS_principle) in mind.
Sylph is fast and extremely reliable, designed for MMO scale Networking.
Sylph uses framing, so anything sent will be received the same way.
Sylph is raw C++ and can be used in KapEngine too.
Thanks to the based version [Github](https://github.com/vis2k/Telepathy)

## What makes Sylph special?

- Concurrent: Telepathy uses one thread per connection. It can make heavy use of multi core processors.
- Simple: Telepathy wraps all the insanity behind Connect/Send/Disconnect/Tick.

## Credits
vis2k & Paul - for [Telepathy](https://github.com/vis2k/Telepathy)
