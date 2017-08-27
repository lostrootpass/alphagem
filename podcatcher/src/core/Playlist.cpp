#include "Playlist.h"

Playlist::Playlist(QString name) : name(name)
{

}

void Playlist::add(Episode* e)
{
	if (!contains(e))
	{
		episodes.push_back(e);

		emit playlistUpdated();
	}
}

void Playlist::addAfterCurrent(Episode* e)
{
	if (!contains(e))
	{
		episodes.insert(1, e);

		emit playlistUpdated();
	}
}

bool Playlist::contains(const Episode* e) const
{
	for (const Episode* ep : episodes)
	{
		if (ep == e)
			return true;
	}

	return false;
}

void Playlist::emplaceFront(Episode* e)
{
	int idx = episodes.indexOf(e);
	if (idx > 0)
		episodes.move(idx, 0);
	else if(idx == -1)
		episodes.push_front(e);
	
	emit playlistUpdated();
}

Episode* Playlist::front() const
{
	if (episodes.size())
		return episodes.first();

	return nullptr;
}

Episode* Playlist::popFront()
{
	Episode* e = episodes.first();
	episodes.pop_front();
	
	emit playlistUpdated();

	return e;
}

void Playlist::remove(Episode* e)
{
	episodes.removeOne(e);

	emit playlistUpdated();
}
