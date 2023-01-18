import java.util.ArrayList;

public class PlaylistNodePrimaryIndex extends PlaylistNode {
	private ArrayList<Integer> audioIds;
	private ArrayList<PlaylistNode> children;
	
	public PlaylistNodePrimaryIndex(PlaylistNode parent) {
		super(parent);
		audioIds = new ArrayList<Integer>();
		children = new ArrayList<PlaylistNode>();
		this.type = PlaylistNodeType.Internal;
	}
	
	public PlaylistNodePrimaryIndex(PlaylistNode parent, ArrayList<Integer> audioIds, ArrayList<PlaylistNode> children) {
		super(parent);
		this.audioIds = audioIds;
		this.children = children;
		this.type = PlaylistNodeType.Internal;
	}
	
	// GUI Methods - Do not modify
	public ArrayList<PlaylistNode> getAllChildren()
	{
		return this.children;
	}
	
	public PlaylistNode getChildrenAt(Integer index) {return this.children.get(index); }
	
	public Integer audioIdCount()
	{
		return this.audioIds.size();
	}
	public Integer audioIdAtIndex(Integer index) {
		if(index >= this.audioIdCount() || index < 0) {
			return -1;
		}
		else {
			return this.audioIds.get(index);
		}
	}
	
	// Extra functions if needed

	public ArrayList<Integer> getAllAudioIds() {
		return this.audioIds;
	}

	public void addAudio(int index, int id){
		audioIds.add(index,id);
	}
	public void addAudio(int id){
		audioIds.add(id);
	}

	public void addAudioInOrder(int id){
		int i = 0;
		while(i < audioIds.size() && audioIds.get(i) <= id)
			i++;
		audioIds.add(i, id);
	}

	public void removeAudioByGivenIndex(int index){audioIds.remove(index);}

	public void addChild(int index, PlaylistNode child){
		this.children.add(index,child);
	}
	public void addChild(PlaylistNode child){
		this.children.add(child);
	}

	public void removeChildByGivenIndex(int index){ this.children.remove(index);}
	public void deployNewChild(PlaylistNode givenChild, PlaylistNode newChild){
		int index;
		for ( index =0; index<this.children.size();index++)
			if (this.children.get(index).equals(givenChild)){
				addChild(index+1, newChild);
				break;
			}
	}

}
