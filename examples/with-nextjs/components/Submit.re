%raw
"require('../styles/main.css')";

open GraphqlHooks.Types;
open Util.ReactStuff;

module CreatePostConfig = [%graphql
  {|
    mutation createPost($title: String!, $url: String!) {
      createPost(title: $title, url: $url) {
        id
        title
        votes
        url
        createdAt
      }
    }
  |}
];

module CreatePostMutation = GraphqlHooksMutation.Make(CreatePostConfig);

[@react.component]
let make = (~onSubmission: unit => unit) => {
  let (title, setTitle) = React.useState(_ => "");
  let (url, setUrl) = React.useState(_ => "");

  let ({loading}, createPost) =
    CreatePostMutation.use(
      ~variables=CreatePostConfig.make(~title, ~url, ())##variables,
      (),
    );

  let reset = () => {
    setTitle(_ => "");
    setUrl(_ => "");
  };

  let handleSubmit = e => {
    e->ReactEvent.Form.preventDefault;
    reset();
    createPost()
    |> Js.Promise.then_(_result => onSubmission() |> Js.Promise.resolve)
    |> ignore;
  };

  <form onSubmit={e => handleSubmit(e)}>
    <Text.H1> "Submit"->s </Text.H1>
    <input
      className="bg-white focus:outline-0 focus:shadow-outline border border-gray-300 rounded-lg py-2 px-2 block appearance-none leading-tight mb-2"
      placeholder="title"
      name="title"
      type_="text"
      value=title
      required=true
      onChange={e => e->ReactEvent.Form.target##value->setTitle}
    />
    <input
      className="bg-white focus:outline-0 focus:shadow-outline border border-gray-300 rounded-lg py-2 px-2 block appearance-none leading-tight mb-2"
      placeholder="url"
      name="url"
      type_="url"
      value=url
      required=true
      onChange={e => e->ReactEvent.Form.target##value->setUrl}
    />
    <button
      type_="submit"
      className="bg-blue-400 hover:bg-blue-700 text-white py-1 px-2 rounded focus:outline-none focus:shadow-outline text-sm">
      {loading ? "Loading..." : "Submit"}->s
    </button>
  </form>;
};